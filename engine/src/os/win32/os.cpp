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
        static_cast<int>((GetSystemMetrics(SM_CXSCREEN) - mWidth) / 2),
        static_cast<int>((GetSystemMetrics(SM_CYSCREEN) - mHeight) / 2),
        static_cast<int>(mWidth),
        static_cast<int>(mHeight),
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
    if (mHwnd != nullptr)
    {
        DestroyWindow(mHwnd);
    }

    if (mHInstance != nullptr)
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
    PeekMessage(&mMsg, NULL, 0, 0, PM_REMOVE);

    return static_cast<Message>(mMsg.message);
}

void Win32Window::dispatchMessage()
{
    TranslateMessage(&mMsg);
    DispatchMessage(&mMsg);
}

LRESULT Win32Window::windowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_SIZE:
    case WM_EXITSIZEMOVE:
        PostMessage(hwnd, static_cast<UINT32>(Message::RESIZE), wParam, lParam);
        break;
    case WM_KEYDOWN:
        if (VK_ESCAPE == wParam)
        {
            PostMessage(hwnd, static_cast<UINT32>(Message::QUIT), wParam, lParam);
        }
        break;
    case WM_CLOSE:
        PostMessage(hwnd, static_cast<UINT32>(Message::QUIT), wParam, lParam);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}
} // namespace ts