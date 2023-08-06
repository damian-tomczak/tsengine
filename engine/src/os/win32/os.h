#pragma once

#include "core/window.h"

#define LIBRARY_TYPE HMODULE
#define LoadFunction GetProcAddress

namespace ts
{
class Win32Window final : public Window
{
public:
    Win32Window(size_t width, size_t height) : Window{width, height}
    {}
    ~Win32Window();

    HINSTANCE getHInstance() const { return mHInstance; }
    HWND getHwnd() const { return mHwnd; }

    void show() override;
    Message peekMessage() override;
    void dispatchMessage() override;

private:
    static LRESULT windowProcedure(HWND pHwnd, UINT msg, WPARAM pWParam, LPARAM wLParam);

    HINSTANCE mHInstance{GetModuleHandle(nullptr)};
    HWND mHwnd{};
    MSG mMsg{};

    void createWindow() override;
};
} // namespace ts