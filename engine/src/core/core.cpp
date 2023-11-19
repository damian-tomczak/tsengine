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

namespace ts
{
inline namespace TS_VER
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
    player.addComponent<ts::RigidBodyComponent>(7.f);
    
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
    while (loop)
    {
        const auto nowTime = std::chrono::high_resolution_clock::now();
        const long long elapsedNanoseconds =
            std::chrono::duration_cast<std::chrono::nanoseconds>(nowTime - previousTime).count();
        static constexpr auto nanosecondsPerSecond = 1e9f;
        const auto dt = static_cast<float>(elapsedNanoseconds) / nanosecondsPerSecond;
        previousTime = nowTime;

#ifdef TESTER_ADAPTER 
        if ((testerAdapter != nullptr) && isRenderingStarted)
        {
            if (std::chrono::steady_clock::now() >= (startTime + testerAdapter->renderingDuration))
            {
                loop = false;
            }
        }
#endif

        if ((!game->tick(dt)) || headset.isExitRequested())
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

        gReg.update();

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

            controllers.sync(headset.getXrSpace(), headset.getXrFrameState().predictedDisplayTime);
            gReg.getSystem<MovementSystem>().update(dt, controllers);

            renderer.render(swapchainImageIndex);
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
} // namespace ver
} // namespace ts
