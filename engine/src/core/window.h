#pragma once

#include "internal_utils.h"

#ifdef _WIN32
    #include <Windows.h>
#endif // _WIN32

namespace ts
{
class Window
{
    TS_NOT_COPYABLE_AND_MOVEABLE(Window);

public:
    Window(const std::string_view windowName, const size_t width, const size_t height) :
        mWindowName{windowName.data() + " powered by tsengine"s}, mWidth{width}, mHeight{height}
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

    static std::shared_ptr<Window> createWindowInstance(const std::string_view windowName, const size_t width, const size_t height);

    [[nodiscard]] size_t getWidth() const { return mWidth; }
    [[nodiscard]] size_t getHeight() const { return mHeight;}

protected:
    const size_t mWidth;
    const size_t mHeight;
    const std::string mWindowName;
};
} // namespace ts