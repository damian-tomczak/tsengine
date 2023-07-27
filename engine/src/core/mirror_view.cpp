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

MirrorView::~MirrorView()
{
    const auto device{mCtx->getVkDevice()};
    if ((device != nullptr) && (mSwapchain != nullptr))
    {
        vkDestroySwapchainKHR(device, mSwapchain, nullptr);
    }

    const auto instance{mCtx->getVkInstance()};
    if ((instance != nullptr) && (mSurface != nullptr))
    {
        vkDestroySurfaceKHR(instance, mSurface, nullptr);
    }
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
    mCtx->sync();

    getSurfaceCapabilitiesAndExtent();

    if (isWindowMinimize())
    {
        return;
    }

    pickSurfaceFormat();
    createSwapchain();
}

bool MirrorView::isWindowMinimize()
{
    if ((mSwapchainResolution.width == 0) || (mSwapchainResolution.height == 0))
    {
        return true;
    }

    return false;
}

void MirrorView::getSurfaceCapabilitiesAndExtent()
{
    const auto physicalDevice{mCtx->getVkPhysicalDevice()};

    LOGGER_VK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR, physicalDevice, mSurface, &mSurfaceCapabilities);

    if (!(mSurfaceCapabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT))
    {
        LOGGER_ERR("Invalid surface");
    }

    if ((mSurfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) &&
        (mSurfaceCapabilities.currentExtent.height != std::numeric_limits<uint32_t>::max()))
    {
        mSwapchainResolution = mSurfaceCapabilities.currentExtent;
    }
    else
    {
        std::shared_ptr<Window> window;
        if (window = mWindow.lock(); window == nullptr)
        {
            LOGGER_ERR("Invalid window");
        }
        auto width{static_cast<uint32_t>(window->getWidth())};
        auto height{static_cast<uint32_t>(window->getHeight())};

        mSwapchainResolution.width = std::clamp(width, static_cast<uint32_t>(mSurfaceCapabilities.minImageExtent.width),
            static_cast<uint32_t>(mSurfaceCapabilities.maxImageExtent.width));
        mSwapchainResolution.height = std::clamp(height, static_cast<uint32_t>(mSurfaceCapabilities.minImageExtent.height),
            static_cast<uint32_t>(mSurfaceCapabilities.maxImageExtent.height));
    }
}

void MirrorView::pickSurfaceFormat()
{
    const auto physicalDevice{ mCtx->getVkPhysicalDevice() };

    uint32_t surfaceFormatCount{};
    LOGGER_VK(vkGetPhysicalDeviceSurfaceFormatsKHR, physicalDevice, mSurface, &surfaceFormatCount, nullptr);

    std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
    LOGGER_VK(vkGetPhysicalDeviceSurfaceFormatsKHR, physicalDevice, mSurface, &surfaceFormatCount, surfaceFormats.data());

    bool surfaceFormatFound{};
    for (const VkSurfaceFormatKHR& surfaceFormatCandidate : surfaceFormats)
    {
        if (surfaceFormatCandidate.format == colorFormat)
        {
            mSurfaceFormat = surfaceFormatCandidate;
            surfaceFormatFound = true;
            break;
        }
    }

    if (!surfaceFormatFound)
    {
        LOGGER_ERR("Surface format isn't available");
    }
}

void MirrorView::createSwapchain()
{
    const auto device{mCtx->getVkDevice()};

    if (mSwapchain != nullptr)
    {
        vkDestroySwapchainKHR(device, mSwapchain, nullptr);
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo{
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = mSurface,
        .minImageCount = mSurfaceCapabilities.minImageCount + 1,
        .imageFormat = mSurfaceFormat.format,
        .imageColorSpace = mSurfaceFormat.colorSpace,
        .imageExtent = mSurfaceCapabilities.currentExtent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .preTransform = mSurfaceCapabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .clipped = VK_TRUE,
    };
    LOGGER_VK(vkCreateSwapchainKHR, device, &swapchainCreateInfo, nullptr, &mSwapchain);

    uint32_t swapchainImageCount{};
    LOGGER_VK(vkGetSwapchainImagesKHR, device, mSwapchain, &swapchainImageCount, nullptr);

    mSwapchainImages.resize(swapchainImageCount);
    LOGGER_VK(vkGetSwapchainImagesKHR, device, mSwapchain, &swapchainImageCount, mSwapchainImages.data());
}
}
