#include "tsengine/core.h"
#include "context.h"
#include "core/window.h"
#include "vulkan/vulkan_functions.h"
#include "tsengine/logger.h"

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

    std::unique_ptr<uint32_t> width{ std::make_unique<uint32_t>(1280) };
    std::unique_ptr<uint32_t> height{ std::make_unique<uint32_t>(720) };
    pEngine->init(width.get(), height.get());

    if (!std::filesystem::is_directory("assets"))
    {
        LOGGER_ERR("assets can not be found");
    }

    auto& ctx{Context::getInstance()};
    ctx.createContext();
    auto pWindow{ Window::createWindow(*width, *height) };
    pWindow->show();

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
catch (...)
{
    return EXIT_FAILURE;
}
} // namespace ts
