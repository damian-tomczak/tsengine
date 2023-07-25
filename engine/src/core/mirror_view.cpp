#include "mirror_view.h"
#include "context.h"
#include "tsengine/logger.h"
#include "os.h"
#include "vulkan_tools/vulkan_functions.h"

namespace ts
{
MirrorView::MirrorView(const Context* pCtx, const std::shared_ptr<Window> pWindow) :
    mpCtx(pCtx),
    mpWindow(pWindow)
{
    createSurface();
}
void MirrorView::createSurface()
{
    const auto pWindow = mpWindow.lock();
    if (pWindow == nullptr)
    {
        LOGGER_ERR("invalid window");
    }

#ifdef VK_USE_PLATFORM_WIN32_KHR

    auto pWinWindow{dynamic_cast<const Win32Window*>(pWindow.get())};

    if (pWinWindow == nullptr)
    {
        LOGGER_ERR("invalid window");
    }

    const VkWin32SurfaceCreateInfoKHR ci{
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .hinstance = pWinWindow->getHInstance(),
        .hwnd = pWinWindow->getHwnd()
    };

    LOGGER_VK(vkCreateWin32SurfaceKHR, mpCtx->getVkInstance(), &ci, nullptr, &mSurface);
#else
#error not implemented
#endif
}
}
