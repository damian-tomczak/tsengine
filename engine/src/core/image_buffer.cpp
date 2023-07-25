#include "image_buffer.h"

#include "context.h"
#include "tsengine/logger.h"
#include "khronos_utils.hpp"

namespace ts
{
ImageBuffer::~ImageBuffer()
{
    const auto device{mpCtx->getVkDevice()};
    if (mImageView)
    {
        vkDestroyImageView(device, mImageView, nullptr);
    }

    if (mDeviceMemory)
    {
        vkFreeMemory(device, mDeviceMemory, nullptr);
    }

    if (mImage)
    {
        vkDestroyImage(device, mImage, nullptr);
    }
}

void ImageBuffer::createImage(
    VkExtent2D size,
    VkFormat format,
    VkImageUsageFlagBits usage,
    VkSampleCountFlagBits samples,
    VkImageAspectFlags aspect,
    size_t layerCount)
{
    const VkImageCreateInfo imageCreateInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = {
            .width = size.width,
            .height = size.height,
            .depth = 1,
        },
        .mipLevels = 1,
        .arrayLayers = static_cast<uint32_t>(layerCount),
        .samples = samples,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = static_cast<VkImageUsageFlags>(usage),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    const VkDevice device{mpCtx->getVkDevice()};

    LOGGER_VK(vkCreateImage, device, &imageCreateInfo, nullptr, &mImage);

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(device, mImage, &memoryRequirements);

    uint32_t suitableMemoryTypeIndex{};
    if (!khronos_utils::findSuitableMemoryTypeIndex(
        mpCtx->getVkPhysicalDevice(),
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
    LOGGER_VK(vkAllocateMemory, device, &memoryAllocateInfo, nullptr, &mDeviceMemory);
    LOGGER_VK(vkBindImageMemory, device, mImage, mDeviceMemory, 0);

    VkImageViewCreateInfo imageViewCreateInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = mImage,
        .viewType = (layerCount == 1 ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_2D_ARRAY),
        .format = format,
        .components = {
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY
        },
        .subresourceRange = {
            .aspectMask = aspect,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = static_cast<uint32_t>(layerCount),
        }
    };

    LOGGER_VK(vkCreateImageView, device, &imageViewCreateInfo, nullptr, &mImageView);
}
}
