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
#include "tests_core_adapter.h"

std::mutex engineInit;

namespace
{
constexpr float flySpeedMultiplier{15.f};

unsigned tickCount{};
bool isAlreadyInitiated{};

void runCleaner()
{
    isAlreadyInitiated = false;
}
} // namespace


// TODO: maybe would be possible to fancy implement individualtons here?
namespace ts
{
[[deprecated("not implemented yet")]]
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
    isAlreadyInitiated = true;

    const auto testerAdapter = dynamic_cast<TesterEngine*>(engine);

    unsigned width{1280u}, height{720u};
    engine->init(width, height);

    if (!std::filesystem::is_directory("assets"))
    {
        LOGGER_ERR("Assets can not be found");
    }

    compileShaders("assets/shaders");

    Context ctx;
    ctx.createOpenXrContext().createVulkanContext();

    auto window = Window::createWindowInstance(GAME_NAME, width, height);
    MirrorView mirrorView{ctx, window};
    mirrorView.createSurface();
    ctx.createVkDevice(mirrorView.getSurface());
    Headset headset{ctx};
    headset.init();
    Controllers controllers(ctx.getXrInstance(), headset.getXrSession());
    controllers.setupControllers();

    std::shared_ptr<Model> ruins = std::make_shared<Model>(Model{
        .model = math::Mat4(1.f),
        .pipeline = PipelineType::NORMAL_LIGHTING,
    });
    std::shared_ptr<Model> polonez = std::make_shared<Model>(Model{
        .pos = {0.f, 0.f, -10.f},
        .model = math::Mat4(1.f),
        .pipeline = PipelineType::NORMAL_LIGHTING,
    });
    std::shared_ptr<Model> sphere1 = std::make_shared<Model>(Model{
        .pos = {0.f, 1.5f, -5.f},
        .model = math::Mat4(1.f),
        .pipeline = PipelineType::PBR,
        .material = Materials::at("Gold"),
    });

    const std::vector<std::shared_ptr<Model>> models{
        ruins,
        polonez,
        sphere1,
    };

    auto meshData = std::make_unique<MeshData>();
    meshData->loadModel("assets/models/ruins.obj", models, 1);
    meshData->loadModel("assets/models/polonez.obj", models, 1);
    meshData->loadModel("assets/models/sphere.obj", models, 1);

    Renderer renderer{ctx, headset, models, std::move(meshData)};
    renderer.createRenderer();
    mirrorView.connect(&headset, &renderer);

    LOGGER_LOG("tsengine initialization completed successfully");

    window->show();
    bool isRenderingStarted{};
    math::Vec3 cameraPosition{};
    auto loop = true;
    auto previousTime = std::chrono::high_resolution_clock::now();
    auto startTime = std::chrono::steady_clock::now();
    // TODO: display message to wear the headset
    // TODO: consider if we should provide an option to render firstly to the window then copy it to the headset
    while (loop)
    {
        // I have no idea how to better implement it.
        if ((testerAdapter != nullptr) && isRenderingStarted)
        {
            if (std::chrono::steady_clock::now() >= (startTime + testerAdapter->renderingDuration))
            {
                loop = false;
            }
        }

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
            if (!isRenderingStarted)
            {
                isRenderingStarted = true;
            }

            controllers.sync(headset.getXrSpace(), headset.getXrFrameState().predictedDisplayTime);

            for (size_t controllerIndex{}; controllerIndex < controllers.controllerCount; ++controllerIndex)
            {
                const auto flySpeed = controllers.getFlySpeed(controllerIndex);
                if (flySpeed > 0.f)
                {
                    const auto controllerPose = controllers.getPose(controllerIndex)[2];

                    if (!controllerPose.isNan())
                    {
                        const math::Vec3 forward{controllers.getPose(controllerIndex)[2]};
                        cameraPosition += forward * flySpeed * flySpeedMultiplier * deltaTime;
                    }
                    else
                    {
                        LOGGER_WARN(std::format("Controller no. {} can not be located.", controllerIndex).c_str());
                    }
                }
            }

            renderer.render(cameraPosition, swapchainImageIndex);
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
            headset.endFrame(frameResult == Headset::BeginFrameResult::RENDER_SKIP_PARTIALLY);
        }
    }

    engine->close();
    ctx.sync();
    isAlreadyInitiated = false;

    return EXIT_SUCCESS;
}
TS_CATCH_FALLBACK_WITH_CLEANER(runCleaner)
} // namespace ts
