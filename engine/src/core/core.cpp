#include "tsengine/core.h"
#include "context.h"
#include "window.h"
#include "vulkan/vulkan_functions.h"
#include "tsengine/logger.h"
#include "mirror_view.h"
#include "headset.h"
#include "controllers.h"

unsigned tickCount{};
bool isAlreadyInitiated{};

std::mutex engineInit;

namespace ts
{
unsigned getTickCount()
{
    return tickCount;
}

int run(Engine* const pEngine) try
{
    std::lock_guard<std::mutex> _{engineInit};

    if (!pEngine)
    {
        LOGGER_ERR("game is unallocated");
    }

    if (isAlreadyInitiated)
    {
        LOGGER_ERR("game is already initiated");
    }

    unsigned width{1280}, height{720};
    pEngine->init(width, height);

    if (!std::filesystem::is_directory("assets"))
    {
        LOGGER_ERR("assets can not be found");
    }

    compileShaders("assets/shaders");

    Context ctx;
    ctx.createOpenXrContext();
    ctx.createVulkanContext();

    auto pWindow{Window::createWindowInstance(width, height)};
    MirrorView mirrorView{ctx, pWindow};
    ctx.createVkDevice(mirrorView.getSurface());
    Headset headset(ctx);
    headset.createRenderPass();
    headset.createXrSession();
    headset.createSwapchain();
    Controllers controllers(ctx.getXrInstance(), headset.getXrSession());
    controllers.setupControllers();

    isAlreadyInitiated = true;

    LOGGER_LOG("tsengine initialization completed successfully");

    pWindow->show();
    while (true /*!pEngine->tick()*/)
    {
        auto message{pWindow->peekMessage()};
        (void)message;
        if (false)
        {
            pEngine->onMouseMove(-1, -1, -1, -1);
        }

        if (false)
        {
            pEngine->onMouseButtonClick({}, false);
        }

        if (false)
        {
            pEngine->onKeyPressed({});
        }

        if (false)
        {
            pEngine->onKeyReleased({});
        }
    }

    pEngine->close();
    isAlreadyInitiated = false;

    return EXIT_SUCCESS;
}
TS_CATCH_FALLBACK
} // namespace ts
