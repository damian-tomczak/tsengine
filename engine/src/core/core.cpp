#include "tsengine/core.h"

#include "globals.hpp"

#include "tsengine/asset_store.h" 
#include "context.h"
#include "window.h"
#include "tsengine/logger.h"
#include "mirror_view.h"
#include "headset.h"
#include "controllers.h"
#include "vulkan_tools/shaders_compiler.h"
#include "renderer.h"
#include "tests_core_adapter.h"

#include "tsengine/ecs/ecs.h" 
#include "ecs/systems/movement_system.hpp" 
#include "ecs/systems/render_system.hpp"

#ifdef CYBSDK_FOUND
    #include "CVirt.h"
#endif

namespace ts
{
bool Engine::init(const char*& gameName, unsigned& width, unsigned& height)
{
    return true;
}

void Engine::close()
{}

namespace
{
    std::mutex engineInit;
    const std::string_view defaultGameName{"Awesome unamed game"};
    bool isAlreadyInitiated{};

    __forceinline void runCleaner()
    {
        isAlreadyInitiated = false;
    }
} // namespace

// TODO: maybe would be possible to fancy break down run function?
int run(Engine* const game) try
{
    std::lock_guard<std::mutex> _{engineInit};

    if (!game)
    {
        TS_ERR("Game pointer is invalid");
    }

    if (isAlreadyInitiated)
    {
        TS_ERR("Game is already initialized");
    }
    isAlreadyInitiated = true;

#ifdef TESTER_ADAPTER
    const auto testerAdapter = dynamic_cast<TesterEngine*>(game);
    bool isRenderingStarted{};
#endif

    unsigned width{1280}, height{720};
    const char* gameName = nullptr;
    if (!game->init(gameName, width, height))
    {
        TS_ERR("Game initialization unsuccessful");
    }

    if (gameName == nullptr)
    {
        gameName = defaultGameName.data();
        TS_WARN(("Game name wasn't set! Default game name selected: "s + gameName).c_str());
    }

    if (!std::filesystem::is_directory("assets"))
    {
        TS_ERR("Assets can not be found");
    }

    compileShaders("assets/shaders");

    auto player = gReg.createEntity();
    player.setTag("player");
    player.addComponent<ts::TransformComponent>();
    player.addComponent<ts::RigidBodyComponent>(2.f);
    
    auto grid = gReg.createEntity();
    grid.setTag("grid");
    grid.addComponent<ts::RendererComponent<PipelineType::GRID>>();

    // TODO: try to delay it
    game->loadLvL();

    Context ctx{gameName};
    ctx.createOpenXrContext().createVulkanContext();

    gReg.addSystem<AssetStore>();
    gReg.addSystem<MovementSystem>();
    gReg.addSystem<RenderSystem>(ctx.getUniformBufferOffsetAlignment());

    gReg.update();

    AssetStore::Models::load();

    auto window = Window::createWindowInstance(gameName, width, height);
    MirrorView mirrorView{ctx, window};
    mirrorView.createSurface();
    ctx.createVkDevice(mirrorView.getSurface());
    Headset headset{ctx};
    headset.init();
    Controllers controllers(ctx.getXrInstance(), headset.getXrSession());
    controllers.setupControllers();

    Renderer renderer{ctx, headset};
    renderer.createRenderer();
    mirrorView.connect(&headset, &renderer);

    TS_LOG("tsengine initialization completed successfully");

    window->show();
    auto loop = true;
    auto previousTime = std::chrono::high_resolution_clock::now();
    auto startTime = std::chrono::steady_clock::now();
    // TODO: firstly render to the window then copy to the headset

#ifdef CYBSDK_FOUND
    const auto device = CybSDK::Virt::FindDevice();
    if (device == nullptr)
    {
        LOGGER_ERR("Cyberith Virtualizer device not found");
    }

    const auto info = device->GetDeviceInfo();

    const auto virtName = info.ProductName;
    const auto virtNameLen = wcslen(virtName);
    std::vector<char> virtBuf(virtNameLen);
    wcstombs(virtBuf.data(), virtName, virtNameLen);
    std::string virtConvertedName(virtBuf.begin(), virtBuf.end());
    LOGGER_LOG(std::format("Device found {} Firmware Version: {}.{}", virtConvertedName, static_cast<int>(info.MajorVersion), static_cast<int>(info.MinorVersion)).c_str());


    if (!device->Open())
    {
        LOGGER_ERR("Unable to connect to Cyberith Virtualizer");
    }
#endif

    while (loop)
    {
#ifdef TESTER_ADAPTER
        if ((testerAdapter != nullptr) && isRenderingStarted)
        {
            if (std::chrono::steady_clock::now() >= (startTime + testerAdapter->renderingDuration))
            {
                loop = false;
            }
        }
#endif

        if ((!game->tick()) || headset.isExitRequested())
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
        static constexpr auto nanosecondsPerSecond = 1e9f;
        const auto deltaTime = static_cast<float>(elapsedNanoseconds) / nanosecondsPerSecond;
        previousTime = nowTime;

        gReg.update();

        gReg.getSystem<MovementSystem>().update(deltaTime);
        gReg.getSystem<RenderSystem::Lights>().update();

        uint32_t swapchainImageIndex;
        const auto frameResult = headset.beginFrame(swapchainImageIndex);
        if (frameResult == Headset::BeginFrameResult::RENDER_FULLY)
        {
#ifdef TESTER_ADAPTER
            if (!isRenderingStarted)
            {
                isRenderingStarted = true;
            }
#endif

#ifdef CYBSDK_FOUND
            const auto ringHeight = device->GetPlayerHeight();
            auto ringAngle = device->GetPlayerOrientation();
            const auto movementDirection = device->GetMovementDirection();
            const auto movementSpeed = device->GetMovementSpeed();

            if (movementSpeed > 0.f)
            {
                ringAngle *= 2 * std::numbers::pi_v<float>;
                auto offsetX = std::sin(ringAngle) * movementSpeed * flySpeedMultiplier * deltaTime;
                auto offsetZ = -(std::cos(ringAngle) * movementSpeed * flySpeedMultiplier * deltaTime);

                if (movementDirection == -1.f)
                {
                    offsetX *= -1;
                    offsetZ *= -1;
                }
                cameraPosition.x += offsetX;
                cameraPosition.z += offsetZ;
            }
#else
            controllers.sync(headset.getXrSpace(), headset.getXrFrameState().predictedDisplayTime);

            for (size_t controllerIndex{}; controllerIndex < controllers.controllerCount; ++controllerIndex)
            {
                const auto flyState = controllers.getFlyState(controllerIndex);
                if (flyState)
                {
                    const auto controllerPose = controllers.getPose(controllerIndex)[2];

                    if ((!controllerPose.isNan()) || (controllerPose == math::Vec3{0.f}))
                    {
                        const math::Vec3 forward{controllers.getPose(controllerIndex)[2]};
                        player.getComponent<TransformComponent>().pos += forward * player.getComponent<RigidBodyComponent>().velocity * deltaTime;
                    }
                    else
                    {
                        TS_WARN(std::format("Controller no. {} can not be located.", controllerIndex).c_str());
                    }
                }
            }
#endif

            renderer.render(player.getComponent<TransformComponent>().pos, swapchainImageIndex);
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

    game->close();
    ctx.sync();
    isAlreadyInitiated = false;

    return EXIT_SUCCESS;
}
TS_CATCH_FALLBACK_WITH_CLEANER(runCleaner)
} // namespace ts
