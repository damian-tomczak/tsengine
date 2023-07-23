#include "window.h"
#include "os.h"

namespace ts
{
std::unique_ptr<Window> Window::createWindowInstance(uint32_t width, uint32_t height)
{
    static bool isWindowAlreadyCreated{};
    std::unique_ptr<Window> pWindow;

#ifdef _WIN32
    pWindow = std::make_unique<Win32Window>(width, height);
#else
    #error "not implemented"
#endif // _WIN32

    pWindow->createWindow();

    isWindowAlreadyCreated = true;

    return pWindow;
}
} // namespace ts