#pragma once

#include "pch.h"
#include "tools.h"
#include <Windows.h>

namespace ts
{
class Window
{
    NOT_COPYABLE_AND_MOVEABLE(Window);

public:
    Window() = default;

    enum class Message
    {
#ifdef WIN32
        RESIZE = WM_USER + 1,
#else
        RESIZE,
#endif
        QUIT
    };

    virtual void show() = 0;
    virtual Message peekMessage() = 0;

    static std::unique_ptr<Window> createWindow(const std::string_view& windowName);
};

class Win32Window final : public Window
{
public:
    Win32Window(const std::string_view& windowName);
    ~Win32Window();

private:
    virtual void show() override;
    virtual Message peekMessage() override;

    static LRESULT windowProcedure(HWND pHwnd, UINT msg, WPARAM pWParam, LPARAM wLParam);

    HINSTANCE mpHInstance{ GetModuleHandle(nullptr) };
    HWND mpHwnd{};
    uint32_t mWidth{ 1280 };
    uint32_t mHeight{ 720 };
    std::string_view mWindowName;
};
} // namespace ts