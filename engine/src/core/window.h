#pragma once

#include "utils.hpp"

#ifdef _WIN32
    #define NOMINMAX
    #include <Windows.h>
#else
    #error not implemented
#endif // _WIN32

namespace ts
{
class Window
{
    NOT_COPYABLE_AND_MOVEABLE(Window);

public:
    Window(size_t width, size_t height) : mWidth{width}, mHeight{height}
    {}
    virtual ~Window();
    enum class Message
    {
#ifdef _WIN32
        RESIZE = WM_USER + 1,
#else
        RESIZE,
#error not implemented
#endif // _WIN32
        QUIT
    };

    virtual void createWindow() = 0;
    virtual void show() = 0;
    virtual Message peekMessage() = 0;
    virtual void dispatchMessage() = 0;

    static std::shared_ptr<Window> createWindowInstance(size_t width, size_t height);

    [[nodiscard]] size_t getWidth() { return mWidth; }
    [[nodiscard]] size_t getHeight() { return mHeight;}

protected:
    size_t mWidth;
    size_t mHeight;
};
} // namespace ts