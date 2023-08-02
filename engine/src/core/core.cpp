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
        LOGGER_ERR("Game pointer is invalid");
    }

    if (isAlreadyInitiated)
    {
        LOGGER_ERR("Game is already initiated");
    }

    unsigned width{1280u}, height{720u};
    engine->init(width, height);

    if (!std::filesystem::is_directory("assets"))
    {
        LOGGER_ERR("Assets can not be found");
    }

    compileShaders("assets/shaders");

    Context ctx;
    ctx.createOpenXrContext().createVulkanContext();

    auto window = Window::createWindowInstance(width, height);
    MirrorView mirrorView{&ctx, window};
    mirrorView.createSurface();
    ctx.createVkDevice(mirrorView.getSurface());
    Headset headset{&ctx};
    headset.init();
    Controllers controllers(ctx.getXrInstance(), headset.getXrSession());
    controllers.setupControllers();

    Model
        gridModel,
        ruinsModel,
        carModelLeft,
        carModelRight,
        beetleModel,
        handModelLeft,
        handModelRight,
        logoModel;

    std::vector<Model*> models{
        &gridModel,
        &ruinsModel,
        &carModelLeft,
        &carModelRight,
        &beetleModel,
        &handModelLeft,
        &handModelRight,
        &logoModel
    };

    gridModel.worldMatrix = ruinsModel.worldMatrix = math::Mat4(1.f);
    carModelLeft.worldMatrix = math::translate(math::Mat4(1.f), {-3.5f, 0.f, -7.f});
    carModelRight.worldMatrix = math::translate(math::Mat4(1.f), {8.f, 0.f, -15.f});
    beetleModel.worldMatrix = math::translate(math::Mat4(1.f), {-3.5f, 0.f, -0.5f});
    logoModel.worldMatrix = math::translate(math::Mat4(1.f), {0.f, 3.f, -10.f});

    auto meshData = std::make_unique<MeshData>();
    meshData->loadModel("assets/models/Grid.obj", models, 1);
    meshData->loadModel("assets/models/Ruins.obj", models, 1);
    meshData->loadModel("assets/models/Car.obj", models, 2);
    meshData->loadModel("assets/models/Beetle.obj", models, 1);
    meshData->loadModel("assets/models/Hand.obj", models, 2);
    meshData->loadModel("assets/models/Logo.obj", models, 1);

    Renderer renderer{&ctx, &headset, models, std::move(meshData)};
    renderer.createRenderer();
    mirrorView.connect(&headset, &renderer);

    isAlreadyInitiated = true;
    LOGGER_LOG("tsengine initialization completed successfully");

    window->show();
    auto cameraMatrix = math::Mat4(1.f);
    auto loop = true;
    auto previousTime = std::chrono::high_resolution_clock::now();
    while (loop)
    {
        if (headset.isExitRequested())
        {
            loop = false;
        }

        auto message = window->peekMessage();
        if (message == Window::Message::QUIT)
        {
            loop = false;
        }
        else if (message == Window::Message::RESIZE)
        {
            mirrorView.onWindowResize();
        }
        window->dispatchMessage();

        const auto nowTime = std::chrono::high_resolution_clock::now();
        const long long elapsedNanoseconds =
            std::chrono::duration_cast<std::chrono::nanoseconds>(nowTime - previousTime).count();
        constexpr auto nanosecondsPerSecond = 1e9f;
        const auto deltaTime = static_cast<float>(elapsedNanoseconds) / nanosecondsPerSecond;
        previousTime = nowTime;

        uint32_t swapchainImageIndex;
        const auto frameResult = headset.beginFrame(swapchainImageIndex);
        if (frameResult == Headset::BeginFrameResult::RENDER_FULLY)
        {
            controllers.sync(headset.getXrSpace(), headset.getXrFrameState().predictedDisplayTime);

            static float time{0.f};
            time += deltaTime;

            for (size_t controllerIndex{}; controllerIndex < controllers.controllerCount; ++controllerIndex)
            {
                const auto flySpeed = controllers.getFlySpeed(controllerIndex);
                if (flySpeed > 0.f)
                {
                    const math::Vec3 forward{math::normalize(controllers.getPose(controllerIndex)[2])};
                    math::Vec3 t = forward * flySpeed * flySpeedMultiplier * deltaTime;
                    cameraMatrix = math::translate(cameraMatrix, t);
                }
            }

            renderer.render(cameraMatrix, swapchainImageIndex, time);
            const auto mirrorResult = mirrorView.render(swapchainImageIndex);

            const auto isMirrorViewVisible = (mirrorResult == MirrorView::RenderResult::VISIBLE);
            renderer.submit(isMirrorViewVisible);

            if (isMirrorViewVisible)
            {
                mirrorView.present();
            }
        }

        if ((frameResult == Headset::BeginFrameResult::RENDER_FULLY) ||
            (frameResult == Headset::BeginFrameResult::RENDER_SKIP_PARTIALLY))
        {
            headset.endFrame();
        }

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
