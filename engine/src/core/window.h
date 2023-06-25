#pragma once

#include "pch.h"
#include "utils.hpp"

#ifdef _WIN32
    #define NOMINMAX
    #include <Windows.h>
#else
    #error not implemented
#endif

namespace ts
{
class Window
{
    NOT_COPYABLE_AND_MOVEABLE(Window);

public:
    Window(uint32_t width, uint32_t height) :
        mWidth{width},
        mHeight{height}
    {}

    enum class Message
    {
#ifdef _WIN32
        RESIZE = WM_USER + 1,
#else
        RESIZE,
#endif
        QUIT
    };

    virtual void show() = 0;
    virtual Message peekMessage() = 0;

    static std::unique_ptr<Window> createWindow(uint32_t width, uint32_t height);

private:
    uint32_t mWidth;
    uint32_t mHeight;
};
}