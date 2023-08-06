#include "render_target.h"
#include "vulkan_tools/vulkan_functions.h"
#include "khronos_utils.h"

namespace ts
{
RenderTarget::~RenderTarget()
{
    if (mFramebuffer)
    {
        vkDestroyFramebuffer(mDevice, mFramebuffer, nullptr);
    }

    if (mImageView)
    {
        vkDestroyImageView(mDevice, mImageView, nullptr);
    }
}

void RenderTarget::createRenderTarget(
    VkImageView colorImageView,
    VkImageView depthImageView,
    VkExtent2D size,
    VkFormat format,
    VkRenderPass renderPass,
    uint32_t layerCount)
{
    const VkImageViewCreateInfo imageViewCreateInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = mImage,
        .viewType = ((layerCount == 1) ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_2D_ARRAY),
        .format = format,
        .components = {
            VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY
        },
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = layerCount,
        }
    };

    LOGGER_VK(vkCreateImageView, mDevice, &imageViewCreateInfo, nullptr, &mImageView);

    const std::array attachments{colorImageView, depthImageView, mImageView};

    const VkFramebufferCreateInfo framebufferCreateInfo{
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = renderPass,
        .attachmentCount = static_cast<uint32_t>(attachments.size()),
        .pAttachments = attachments.data(),
        .width = size.width,
        .height = size.height,
        .layers = 1
    };

    LOGGER_VK(vkCreateFramebuffer, mDevice, &framebufferCreateInfo, nullptr, &mFramebuffer);
}
}