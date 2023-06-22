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

int run(Engine* const pEngine)
{
    if (!pEngine)
    {
        return EXIT_FAILURE;
    }

    if (isAlreadyInitiated)
    {
        return EXIT_FAILURE;
    }

    const char* pGameName{};
    int width;
    int height;
    bool isFullscreen;
    pEngine->preInit(pGameName, width, height, isFullscreen);

    if (pGameName != nullptr)
    {
        LOGGER_ERR("Game hasn't been named");
    }

    if (!std::filesystem::is_directory("assets"))
    {
        // TODO: logger
    }

    auto pWindow{ Window::createWindow(pGameName) };
    pWindow->show();
    Context ctx{pGameName};

    pEngine->init();
    isAlreadyInitiated = true;

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
} // namespace ts
