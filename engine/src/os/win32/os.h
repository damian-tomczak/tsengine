#pragma once

#include "core/window.h"

#define LIBRARY_TYPE HMODULE
#define LoadFunction GetProcAddress

namespace ts
{
class Win32Window final : public Window
{
public:
    Win32Window(uint32_t width, uint32_t height);
    ~Win32Window();

private:
    virtual void show() override;
    virtual Message peekMessage() override;

    static LRESULT windowProcedure(HWND pHwnd, UINT msg, WPARAM pWParam, LPARAM wLParam);

    HINSTANCE mpHInstance{ GetModuleHandle(nullptr) };
    HWND mpHwnd{};
    uint32_t mWidth;
    uint32_t mHeight;
};
} // namespace ts