#include "mirror_view.h"
#include "context.h"
#include "tsengine/logger.h"
#include "os.h"
#include "vulkan_tools/vulkan_functions.h"
#include "renderer.h"
#include "khronos_utils.h"
#include "headset.h"
#include "render_target.h"

namespace ts
{
MirrorView::MirrorView(const Context& ctx, const std::shared_ptr<Window> window) :
    mCtx{ctx},
    mWindow{window}
{}

MirrorView::~MirrorView()
{
    const auto vkDevice = mCtx.getVkDevice();
    if ((vkDevice != nullptr) && (mSwapchain != nullptr))
    {
        vkDestroySwapchainKHR(vkDevice, mSwapchain, nullptr);
    }

    const auto vkinstance = mCtx.getVkInstance();
    if ((vkinstance != nullptr) && (mSurface != nullptr))
    {
        vkDestroySurfaceKHR(vkinstance, mSurface, nullptr);
    }
}

void MirrorView::connect(const Headset* headset, const Renderer* renderer)
{
    mHeadset = headset;
    mRenderer = renderer;

    recreateXrSwapchain();
}

MirrorView::RenderResult MirrorView::render(uint32_t swapchainImageIndex)
{
    if ((mSwapchainResolution.width == 0) || (mSwapchainResolution.height == 0))
    {
        if (mIsResizeDetected)
        {
            mIsResizeDetected = false;
            recreateXrSwapchain();
        }
        else
        {
            return RenderResult::INVISIBLE;
        }
    }

    const auto result =
        vkAcquireNextImageKHR(
            mCtx.getVkDevice(),
            mSwapchain,
            UINT64_MAX,
            mRenderer->getCurrentDrawableSemaphore(),
            VK_NULL_HANDLE,
            &mDestinationImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateXrSwapchain();

        return RenderResult::INVISIBLE;
    }
    else if ((result != VK_SUBOPTIMAL_KHR) && (result != VK_SUCCESS))
    {
        return RenderResult::INVISIBLE;
    }

    const auto commandBuffer = mRenderer->getCurrentCommandBuffer();
    const auto sourceImage = mHeadset->getRenderTarget(swapchainImageIndex)->getImage();
    const auto destinationImage = mSwapchainImages.at(mDestinationImageIndex);
    const auto eyeResolution = mHeadset->getEyeResolution(mirrorEyeIndex);

    VkImageMemoryBarrier imageMemoryBarrier{
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .image = sourceImage,
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = mirrorEyeIndex,
            .layerCount = 1,
        }
    };
    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_DEPENDENCY_BY_REGION_BIT,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &imageMemoryBarrier);

    imageMemoryBarrier.image = destinationImage;
    imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageMemoryBarrier.srcAccessMask = 0u;
    imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageMemoryBarrier.subresourceRange.layerCount = 1;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier.subresourceRange.levelCount = 1;
    imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_DEPENDENCY_BY_REGION_BIT,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &imageMemoryBarrier);

    const math::Vec2 sourceResolution{static_cast<float>(eyeResolution.width), static_cast<float>(eyeResolution.height)};
    const float sourceAspectRatio{sourceResolution.x / sourceResolution.y};
    const math::Vec2 destinationResolution{static_cast<float>(mSwapchainResolution.width), static_cast<float>(mSwapchainResolution.height) };
    const float destinationAspectRatio = destinationResolution.x / destinationResolution.y;
    math::Vec2 cropResolution = sourceResolution, cropOffset{0.f, 0.f};

    if (sourceAspectRatio < destinationAspectRatio)
    {
        cropResolution.y = sourceResolution.x / destinationAspectRatio;
        cropOffset.y = (sourceResolution.y - cropResolution.y) / 2.f;
    }
    else if (sourceAspectRatio > destinationAspectRatio)
    {
        cropResolution.x = sourceResolution.y * destinationAspectRatio;
        cropOffset.x = (sourceResolution.x - cropResolution.x) / 2.f;
    }

    VkImageBlit imageBlit{
        .srcSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = mirrorEyeIndex,
            .layerCount = 1,
        },
        .srcOffsets = {
            {
                static_cast<int32_t>(cropOffset.x),
                static_cast<int32_t>(cropOffset.y),
                0
            },
            {
                static_cast<int32_t>(cropOffset.x + cropResolution.x),
                static_cast<int32_t>(cropOffset.y + cropResolution.y),
                1
            },
        },
        .dstSubresource = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
        .dstOffsets = {
            {0, 0, 0},
            {
                static_cast<int32_t>(destinationResolution.x),
                static_cast<int32_t>(destinationResolution.y),
                1
            }
        },
    };

    vkCmdBlitImage(
        commandBuffer,
        sourceImage,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        destinationImage,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &imageBlit,
        VK_FILTER_NEAREST);

    imageMemoryBarrier.image = sourceImage;
    imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageMemoryBarrier.subresourceRange.layerCount = 1;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = mirrorEyeIndex;
    imageMemoryBarrier.subresourceRange.levelCount = 1;
    imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_DEPENDENCY_BY_REGION_BIT,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &imageMemoryBarrier);

    imageMemoryBarrier.image = destinationImage;
    imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imageMemoryBarrier.dstAccessMask = 0u;
    imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageMemoryBarrier.subresourceRange.layerCount = 1u;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = 0u;
    imageMemoryBarrier.subresourceRange.levelCount = 1u;
    imageMemoryBarrier.subresourceRange.baseMipLevel = 0u;
    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_DEPENDENCY_BY_REGION_BIT,
        0,
        nullptr,
        0,
        nullptr,
        1,
        &imageMemoryBarrier);

    return RenderResult::VISIBLE;
}

void MirrorView::present()
{
    const auto presentableSemaphore = mRenderer->getCurrentPresentableSemaphore();

    VkPresentInfoKHR presentInfo{
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &presentableSemaphore,
        .swapchainCount = 1,
        .pSwapchains = &mSwapchain,
        .pImageIndices = &mDestinationImageIndex
    };

    const auto result = vkQueuePresentKHR(mCtx.getVkPresentQueue(), &presentInfo);
    if ((result == VK_ERROR_OUT_OF_DATE_KHR) || (result == VK_SUBOPTIMAL_KHR))
    {
        recreateXrSwapchain();
    }
    else if (result != VK_SUCCESS)
    {
        LOGGER_ERR(("vkQueuePresentKHR failed with status: " + khronos_utils::vkResultToString(result)).c_str());
    }
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

    LOGGER_VK(vkCreateWin32SurfaceKHR, mCtx.getVkInstance(), &ci, nullptr, &mSurface);
#else
#error not implemented
#endif
}

void MirrorView::recreateXrSwapchain()
{
    mCtx.sync();

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
    const auto physicalDevice = mCtx.getVkPhysicalDevice();

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
    const auto physicalDevice = mCtx.getVkPhysicalDevice();

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
        LOGGER_ERR("surface format isn't available");
    }
}

void MirrorView::createSwapchain()
{
    const auto device = mCtx.getVkDevice();

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
