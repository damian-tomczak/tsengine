#include "tsengine/core.h"
#include "context.h"
#include "window.h"
#include "tsengine/logger.h"
#include "mirror_view.h"
#include "headset.h"
#include "controllers.h"
#include "vulkan_tools/shaders_compiler.h"
#include "game_object.hpp"
#include "renderer.h"

std::mutex engineInit;

namespace
{
    constexpr float flySpeedMultiplier{2.5f};

    unsigned tickCount{};
    bool isAlreadyInitiated{};
}

namespace ts
{
unsigned getTickCount()
{
    return tickCount;
}

int run(Engine* const engine) try
{
    std::lock_guard<std::mutex> _{engineInit};

    if (!engine)
    {
        LOGGER_ERR("game is unallocated");
    }

    if (isAlreadyInitiated)
    {
        LOGGER_ERR("game is already initiated");
    }

    unsigned width{1280}, height{720};
    engine->init(width, height);

    if (!std::filesystem::is_directory("assets"))
    {
        LOGGER_ERR("assets can not be found");
    }

    compileShaders("assets/shaders");

    Context ctx;
    ctx.createOpenXrContext();
    ctx.createVulkanContext();

    auto window{Window::createWindowInstance(width, height)};
    MirrorView mirrorView{&ctx, window};
    ctx.createVkDevice(mirrorView.getSurface());
    Headset headset{&ctx};
    headset.createRenderPass();
    headset.createXrSession();
    headset.createSwapchain();
    Controllers controllers(ctx.getXrInstance(), headset.getXrSession());
    controllers.setupControllers();

    Model
        gridModel,
        ruinsModel,
        carModelLeft,
        carModelRight,
        beetleModel,
        bikeModel,
        handModelLeft,
        handModelRight,
        logoModel;

    std::vector<Model*> models{
        &gridModel,
        &ruinsModel,
        &carModelLeft,
        &carModelRight,
        &beetleModel,
        &bikeModel,
        &handModelLeft,
        &handModelRight,
        &logoModel
    };

    gridModel.worldMatrix = math::Mat4::makeScalarMat(1.f);
    carModelLeft.worldMatrix = math::translate(math::Mat4::makeScalarMat(1.0f), {-3.5f, 0.0f, -7.0f});
    carModelRight.worldMatrix = math::translate(math::Mat4::makeScalarMat(1.0f), {8.0f, 0.0f, -15.0f});
    beetleModel.worldMatrix = math::translate(math::Mat4::makeScalarMat(1.0f), {-3.5f, 0.0f, -0.5f});
    logoModel.worldMatrix = math::translate(math::Mat4::makeScalarMat(1.0f), {0.0f, 3.0f, -10.0f});

    auto meshData{std::make_unique<MeshData>()};
    meshData->loadModel("assets/models/Grid.obj", models, 1);
    meshData->loadModel("assets/models/Ruins.obj", models, 1);
    meshData->loadModel("assets/models/Car.obj", models, 2);
    meshData->loadModel("assets/models/Beetle.obj", models, 1);
    meshData->loadModel("assets/models/Bike.obj", models, 1);
    meshData->loadModel("assets/models/Hand.obj", models, 2);
    meshData->loadModel("assets/models/Logo.obj", models, 1);

    Renderer renderer{&ctx, &headset, models, std::move(meshData)};
    renderer.createRenderer();
    mirrorView.connect(&headset, &renderer);

    isAlreadyInitiated = true;
    LOGGER_LOG("tsengine initialization completed successfully");

    window->show();

    math::Mat4 cameraMat;
    auto previousTime{std::chrono::high_resolution_clock::now()};
    while (!headset.isExitRequested())
    {
        auto message{window->peekMessage()};
        if (message == Window::Message::QUIT)
        {
            break;
        }

        const auto nowTime{std::chrono::high_resolution_clock::now()};
        const auto deltaTime{std::chrono::duration_cast<std::chrono::seconds>(nowTime - previousTime).count()};
        previousTime = nowTime;

        uint32_t swapchainImageIndex;
        const Headset::BeginFrameResult frameResult = headset.beginFrame(swapchainImageIndex);
        if (frameResult == Headset::BeginFrameResult::RENDER_FULLY)
        {
            controllers.sync(headset.getXrSpace(), headset.getXrFrameState().predictedDisplayTime);

            static float time{0.f};
            time += deltaTime;

            for (size_t controllerIndex = 0u; controllerIndex < 2u; ++controllerIndex)
            {
                const auto flySpeed = controllers.getFlySpeed(controllerIndex);
                if (flySpeed > 0.0f)
                {
                    const math::Vec3 forward(math::normalize(controllers.getPose(controllerIndex)[2]));
                    cameraMat = math::translate(cameraMat, forward * flySpeed * flySpeedMultiplier * deltaTime);
                }
            }

            const auto inverseCameraMat = math::inverse(cameraMat);
            handModelLeft.worldMatrix = inverseCameraMat * controllers.getPose(0);
            handModelRight.worldMatrix = inverseCameraMat * controllers.getPose(1);
            handModelRight.worldMatrix = math::scale(handModelRight.worldMatrix, { -1.0f, 1.0f, 1.0f });


              renderer.render(cameraMat, swapchainImageIndex, time);

        //    const MirrorView::RenderResult mirrorResult = mirrorView.render(swapchainImageIndex);
        //    if (mirrorResult == MirrorView::RenderResult::Error)
        //    {
        //        return EXIT_FAILURE;
        //    }

        //    const bool mirrorViewVisible = (mirrorResult == MirrorView::RenderResult::Visible);
        //    renderer.submit(mirrorViewVisible);

        //    if (mirrorViewVisible)
        //    {
        //        mirrorView.present();
        //    }
        }

        //if (frameResult == Headset::BeginFrameResult::RenderFully || frameResult == Headset::BeginFrameResult::SkipRender)
        //{
        //    headset.endFrame();
        //}

        //if (false)
        //{
        //    engine->onMouseMove(-1, -1, -1, -1);
        //}

        //if (false)
        //{
        //    engine->onMouseButtonClick({}, false);
        //}

        //if (false)
        //{
        //    engine->onKeyPressed({});
        //}

        //if (false)
        //{
        //    engine->onKeyReleased({});
        //}
    }

    engine->close();
    ctx.sync();
    isAlreadyInitiated = false;

    return EXIT_SUCCESS;
}
TS_CATCH_FALLBACK
} // namespace ts
