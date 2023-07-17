#include "tsengine/core.h"
#include "context.h"
#include "window.h"
#include "vulkan/vulkan_functions.h"
#include "tsengine/logger.h"
#include "mirror_view.h"

unsigned tickCount{};
bool isAlreadyInitiated{};

namespace ts
{
unsigned getTickCount()
{
    return tickCount;
}

int run(Engine* const pEngine) try
{
    if (!pEngine)
    {
        LOGGER_ERR("game is unallocated");
    }

    if (isAlreadyInitiated)
    {
        LOGGER_ERR("game is already initiated");
    }

    unsigned width{ 1280 }, height{ 720 };
    pEngine->init(width, height);

    if (!std::filesystem::is_directory("assets"))
    {
        LOGGER_ERR("assets can not be found");
    }

    auto& ctx{Context::getInstance()};
    ctx.createContext();
    auto pWindow{ Window::createWindow(width, height) };
    pWindow->show();
    MirrorView mirrorView{ctx, pWindow};
    ctx.createDevice(mirrorView.getSurface());

    isAlreadyInitiated = true;

    LOGGER_LOG("tsengine initialization completed successfully");

    while (!pEngine->tick())
    {
        auto message{ pWindow->peekMessage() };
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
catch (const TSException&)
{
    return TS_FAILURE;
}
catch (const std::exception& e)
{
    LOGGER_ERR(e.what());
    return STL_FAILURE;
}
catch (...)
{
    return UNKNOWN_FAILURE;
}
} // namespace ts
