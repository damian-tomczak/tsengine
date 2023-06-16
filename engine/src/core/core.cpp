#include "tsengine/core.h"
#include "core/window.h"
#include "vulkan/vulkan.h"

unsigned tickCount{};
auto isAlreadyInitiated{ false };

namespace ts
{
unsigned getTickCount()
{
    return tickCount;
}

int run(Engine* pEngine)
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

    if (!pGameName)
    {
        // TODO: logger
    }

    auto pWindow{ Window::createWindow(pGameName) };
    pWindow->show();

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

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}
} // namespace ts
