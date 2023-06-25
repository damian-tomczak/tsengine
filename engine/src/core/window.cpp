#include "window.h"
#include "os.h"

namespace ts
{
std::unique_ptr<Window> Window::createWindow(uint32_t width, uint32_t height)
{
#ifdef _WIN32
    return std::make_unique<Win32Window>(width, height);
#else
    #error "not implemented"
#endif
}
}