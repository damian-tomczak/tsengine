#include "render_target.h"

namespace ts
{
RenderTarget::~RenderTarget()
{
    if (mVkFramebuffer)
    {
        vkDestroyFramebuffer(mVkDevice, mVkFramebuffer, nullptr);
    }

    if (mVkImageView)
    {
        vkDestroyImageView(mVkDevice, mVkImageView, nullptr);
    }
}

void RenderTarget::createRenderTarget(
    VkImageView vkColorImageView,
    VkImageView vkDepthImageView,
    VkExtent2D vkSize,
    VkFormat vkFormat,
    VkRenderPass vkRenderPass,
    uint32_t layerCount)
{
    const VkImageViewCreateInfo imageViewCreateInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = mVkImage,
        .viewType = ((layerCount == 1u) ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_2D_ARRAY),
        .format = vkFormat,
        .components = {
            VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY
        },
        .subresourceRange = {
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0u,
            .levelCount = 1u,
            .baseArrayLayer = 0u,
            .layerCount = layerCount,
        }
    };

    LOGGER_VK(vkCreateImageView, mVkDevice, &imageViewCreateInfo, nullptr, &mVkImageView);

    const std::array attachments = { vkColorImageView, vkDepthImageView, mVkImageView };

    const VkFramebufferCreateInfo framebufferCreateInfo{
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = vkRenderPass,
        .attachmentCount = static_cast<uint32_t>(attachments.size()),
        .pAttachments = attachments.data(),
        .width = vkSize.width,
        .height = vkSize.height,
        .layers = 1u
    };

    LOGGER_VK(vkCreateFramebuffer, mVkDevice, &framebufferCreateInfo, nullptr, &mVkFramebuffer);
}
}