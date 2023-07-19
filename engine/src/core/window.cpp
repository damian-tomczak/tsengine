#include "window.h"
#include "os.h"

namespace ts
{
std::unique_ptr<Window> Window::createWindow(uint32_t width, uint32_t height)
{
    static bool isWindowAlreadyCreated{};
    std::unique_ptr<Window> window;

#ifdef _WIN32
    window = std::make_unique<Win32Window>(width, height);
#else
    #error "not implemented"
#endif // _WIN32

    isWindowAlreadyCreated = true;

    return window;
}
} // namespace ts