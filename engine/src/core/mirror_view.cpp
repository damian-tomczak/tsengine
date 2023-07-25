#include "mirror_view.h"
#include "context.h"
#include "tsengine/logger.h"
#include "os.h"
#include "vulkan_tools/vulkan_functions.h"
#include "renderer.h"

namespace ts
{
MirrorView::MirrorView(const Context* ctx, const std::shared_ptr<Window> window) :
    mCtx(ctx),
    mWindow(window)
{
    createSurface();
}

void MirrorView::connect(const Headset* headset, const Renderer* renderer)
{
    mHeadset = headset;
    mRenderer = renderer;

    recreateSwapchain();
}

void MirrorView::createSurface()
{
    const auto window = mWindow.lock();
    if (window == nullptr)
    {
        LOGGER_ERR("invalid window");
    }

#ifdef VK_USE_PLATFORM_WIN32_KHR

    auto winWindow{dynamic_cast<const Win32Window*>(window.get())};

    if (winWindow == nullptr)
    {
        LOGGER_ERR("invalid window");
    }

    const VkWin32SurfaceCreateInfoKHR ci{
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .hinstance = winWindow->getHInstance(),
        .hwnd = winWindow->getHwnd()
    };

    LOGGER_VK(vkCreateWin32SurfaceKHR, mCtx->getVkInstance(), &ci, nullptr, &mSurface);
#else
#error not implemented
#endif
}

void MirrorView::recreateSwapchain()
{

}
}
