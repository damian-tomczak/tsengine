#include "window.h"
#include "os/win32/os.h"

namespace ts
{
std::unique_ptr<Window> Window::createWindow(const std::string_view& windowName)
{
#ifdef WIN32
    return std::make_unique<Win32Window>(windowName);
#else
    #pragma message("not implemented")
#endif
}
} // namespace ts