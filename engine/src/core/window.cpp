#include "window.h"
#include "os.h"
#include "tsengine/logger.h"

namespace
{
bool isWindowAlreadyCreated{};
}

namespace ts
{
Window::~Window()
{
    isWindowAlreadyCreated = false;
}

std::shared_ptr<Window> Window::createWindowInstance(const std::string_view windowName, const size_t width, const size_t height)
{
    if (isWindowAlreadyCreated)
    {
        TS_ERR("Window is already created");
    }

    std::shared_ptr<Window> window;

#ifdef _WIN32
    window = std::make_shared<Win32Window>(windowName, width, height);
#else
    #error "not implemented"
#endif // _WIN32

    window->createWindow();

    isWindowAlreadyCreated = true;

    return window;
}
} // namespace ts