#pragma once

#include "core/window.h"

#define LIBRARY_TYPE HMODULE
#define LoadFunction GetProcAddress

namespace ts
{
class Win32Window final : public Window
{
public:
    Win32Window(size_t width, size_t height) : Window{ width, height }
    {}
    ~Win32Window();

    void createWindow() override;

    HINSTANCE getHInstance() const { return mHInstance; }
    HWND getHwnd() const { return mHwnd; }

private:
    virtual void show() override;
    virtual Message peekMessage() override;

    static LRESULT windowProcedure(HWND pHwnd, UINT msg, WPARAM pWParam, LPARAM wLParam);

    HINSTANCE mHInstance{GetModuleHandle(nullptr)};
    HWND mHwnd{};
    uint32_t mWidth;
    uint32_t mHeight;
};
} // namespace ts