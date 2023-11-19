#pragma once

#include "core/window.h"

#define LIBRARY_TYPE HMODULE
#define LoadFunction GetProcAddress

namespace ts
{
inline namespace TS_VER
{
class Win32Window final : public Window
{
public:
    Win32Window(const std::string_view windowName, const size_t width, const size_t height)
        : Window{windowName, width, height}
    {}
    ~Win32Window();

    HINSTANCE getHInstance() const { return mHInstance; }
    HWND getHwnd() const { return mHwnd; }

    void show() override;
    Message peekMessage() override;
    void dispatchMessage() override;

private:
    static LRESULT windowProcedure(HWND pHwnd, UINT msg, WPARAM pWParam, LPARAM wLParam);

    HINSTANCE mHInstance{};
    HWND mHwnd{};
    MSG mMsg{};

    void createWindow() override;
};
} // namespace ver
} // namespace ts