#include "os.h"

namespace ts
{
Win32Window::Win32Window(const std::string_view& windowName) : mWindowName{ windowName }
{
    WNDCLASSEX wc{
        .cbSize{ sizeof(WNDCLASSEX) },
        .style{ CS_HREDRAW | CS_VREDRAW },
        .lpfnWndProc{ windowProcedure },
        .hInstance{ mpHInstance },
        .hCursor{ LoadCursor(nullptr, IDC_ARROW) },
        .hbrBackground{ reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1) },
        .lpszClassName{ "tsengine" },
    };

    if (!RegisterClassEx(&wc))
    {
        // TODO: logger
    }

    mpHwnd = CreateWindow("tsengine", mWindowName.data(), WS_OVERLAPPEDWINDOW,
                          (GetSystemMetrics(SM_CXSCREEN) - mWidth) / 2, (GetSystemMetrics(SM_CYSCREEN) - mHeight) / 2,
                          mWidth, mHeight, nullptr, nullptr, mpHInstance, nullptr);

    if (!mpHwnd)
    {
        // TODO: logger
    }
}

Win32Window::~Win32Window()
{
    if (mpHwnd)
    {
        DestroyWindow(mpHwnd);
    }

    if (mpHInstance)
    {
        UnregisterClass("tsengine", mpHInstance);
    }
}

void Win32Window::show()
{
    ShowWindow(mpHwnd, SW_SHOWDEFAULT);
    UpdateWindow(mpHwnd);
}

Window::Message Win32Window::peekMessage()
{
    MSG msg{};
    PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);

    return static_cast<Message>(msg.message);
}

LRESULT CALLBACK Win32Window::windowProcedure(HWND pHwnd, UINT msg, WPARAM pWParam, LPARAM pLParam)
{
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