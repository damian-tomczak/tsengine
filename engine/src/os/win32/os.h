#pragma once

#include "core/window.h"

#define LIBRARY_TYPE HMODULE

namespace ts
{
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