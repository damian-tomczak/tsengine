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
} // namespace ts