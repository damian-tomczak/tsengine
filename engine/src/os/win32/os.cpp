#include "os.h"
#include "tsengine/logger.h"

namespace ts
{
    void Win32Window::createWindow()
    {
        const WNDCLASSEX wc{
            .cbSize = sizeof(WNDCLASSEX),
            .style = CS_HREDRAW | CS_VREDRAW,
            .lpfnWndProc = windowProcedure,
            .hInstance = mHInstance,
            .hCursor = LoadCursor(nullptr, IDC_ARROW),
            .hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1),
            .lpszClassName = ENGINE_NAME,
        };

        if (!RegisterClassEx(&wc))
        {
            LOGGER_ERR("WNDCLASSEX registration failure");
        }

        mHwnd = CreateWindow(
            ENGINE_NAME,
            GAME_NAME,
            WS_OVERLAPPEDWINDOW,
            (GetSystemMetrics(SM_CXSCREEN) - mWidth) / 2,
            (GetSystemMetrics(SM_CYSCREEN) - mHeight) / 2,
            mWidth,
            mHeight,
            nullptr,
            nullptr,
            mHInstance,
            nullptr);

        if (mHwnd == nullptr)
        {
            LOGGER_ERR("Window creation failure");
        }
    }

Win32Window::~Win32Window()
{
    if (mHwnd)
    {
        DestroyWindow(mHwnd);
    }

    if (mHInstance)
    {
        UnregisterClass("tsengine", mHInstance);
    }
}

void Win32Window::show()
{
    ShowWindow(mHwnd, SW_SHOWDEFAULT);
    UpdateWindow(mHwnd);
}

Window::Message Win32Window::peekMessage()
{
    MSG msg{};
    PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

    return static_cast<Message>(msg.message);
}

LRESULT CALLBACK Win32Window::windowProcedure(HWND pHwnd, UINT msg, WPARAM pWParam, LPARAM pLParam)
{
    // TODO: investigate switch cases
    switch (msg)
    {
    case WM_SIZE:
    case WM_EXITSIZEMOVE:
        PostMessage(pHwnd, static_cast<UINT>(Message::RESIZE), pWParam, pLParam);
        break;
    case WM_KEYDOWN:
        if (pWParam == VK_ESCAPE)
        {
            PostMessage(pHwnd, static_cast<UINT>(Message::QUIT), pWParam, pLParam);
        }
        break;
    case WM_CLOSE:
        PostMessage(pHwnd, static_cast<UINT>(Message::QUIT), pWParam, pLParam);
        break;
    default:
        return DefWindowProc(pHwnd, msg, pWParam, pLParam);
    }

    return 0;
}
} // namespace ts