#include "window.h"
#include "os.h"
#include "tsengine/logger.h"

namespace
{
bool isWindowAlreadyCreated{};
}

namespace ts
{
std::shared_ptr<Window> Window::createWindowInstance(size_t width, size_t height)
{
    if (isWindowAlreadyCreated)
    {
        LOGGER_ERR("window is already created");
    }

    std::shared_ptr<Window> window;

#ifdef _WIN32
    window = std::make_shared<Win32Window>(width, height);
#else
    #error "not implemented"
#endif // _WIN32

    window->createWindow();

    isWindowAlreadyCreated = true;

    return window;
}
} // namespace ts