#include "image_buffer.h"

namespace ts
{
ImageBuffer::~ImageBuffer()
{
    const auto vkDevice{mCtx.getVkDevice()};
    if (mVkImageView)
    {
        vkDestroyImageView(vkDevice, mVkImageView, nullptr);
    }

    if (mVkDeviceMemory)
    {
        vkFreeMemory(vkDevice, mVkDeviceMemory, nullptr);
    }

    if (mVkImage)
    {
        vkDestroyImage(vkDevice, mVkImage, nullptr);
    }
}

void ImageBuffer::createImage(VkExtent2D vkSize,
    VkFormat vkFormat,
    VkImageUsageFlagBits vkUsage,
    VkSampleCountFlagBits vkSamples,
    VkImageAspectFlags vkAspect,
    size_t layerCount)
{
    const VkImageCreateInfo imageCreateInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = vkFormat,
        .extent = {
            .width = vkSize.width,
            .height = vkSize.height,
            .depth = 1u,
        },
        .mipLevels = 1u,
        .arrayLayers = static_cast<uint32_t>(layerCount),
        .samples = vkSamples,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = static_cast<VkImageUsageFlags>(vkUsage),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    const VkDevice vkDevice{ mCtx.getVkDevice() };

    LOGGER_VK(vkCreateImage, vkDevice, &imageCreateInfo, nullptr, &mVkImage);

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(vkDevice, mVkImage, &memoryRequirements);

    uint32_t suitableMemoryTypeIndex{};
    if (!utils::findSuitableMemoryTypeIndex(
        mCtx.getVkPhysicalDevice(),
        memoryRequirements,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        suitableMemoryTypeIndex))
    {
        LOGGER_ERR("suitable memory type couldn't be found");
    }

    const VkMemoryAllocateInfo memoryAllocateInfo{
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = suitableMemoryTypeIndex
    };
    LOGGER_VK(vkAllocateMemory, vkDevice, &memoryAllocateInfo, nullptr, &mVkDeviceMemory);
    LOGGER_VK(vkBindImageMemory, vkDevice, mVkImage, mVkDeviceMemory, 0u);

    VkImageViewCreateInfo imageViewCreateInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = mVkImage,
        .viewType = (layerCount == 1u ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_2D_ARRAY),
        .format = vkFormat,
        .components = {
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY
        },
        .subresourceRange = {
            .aspectMask = vkAspect,
            .baseMipLevel = 0u,
            .levelCount = 1u,
            .baseArrayLayer = 0u,
            .layerCount = static_cast<uint32_t>(layerCount),
        }
    };

    LOGGER_VK(vkCreateImageView, vkDevice, &imageViewCreateInfo, nullptr, &mVkImageView);
}
}
