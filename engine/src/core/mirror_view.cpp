#include "mirror_view.h"

namespace ts
{
MirrorView::MirrorView(Context& pContext, const std::unique_ptr<Window>& pWindow) :
    mpContext{ pContext },
    mpWindow{ pWindow }
{
    createSurface();
}
void MirrorView::createSurface()
{
    auto pWindow{ dynamic_cast<Win32Window*>(mpWindow.get()) };

    if (pWindow == nullptr)
    {
        LOGGER_ERR("invalid window");
    }

#ifdef VK_USE_PLATFORM_WIN32_KHR
    const VkWin32SurfaceCreateInfoKHR ci{
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .hinstance = pWindow->getHInstance(),
        .hwnd = pWindow->getHwnd()
    };

    LOGGER_VK(vkCreateWin32SurfaceKHR, mpContext.getVkInstance(), &ci, nullptr, &mSurface);
#else
#error not implemented
#endif
}
}
