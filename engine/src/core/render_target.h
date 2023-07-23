#pragma once

#include <vulkan/vulkan_loader.h>
#include "utils.hpp"
#include "tsengine/logger.h"

namespace ts
{
class RenderTarget final
{
    NOT_COPYABLE_AND_MOVEABLE(RenderTarget);

public:
    RenderTarget(VkDevice vkDevice, VkImage vkImage) : mVkDevice(vkDevice), mVkImage(vkImage)
    {}
    ~RenderTarget();

    void createRenderTarget(VkImageView vkColorImageView,
        VkImageView vkDepthImageView,
        VkExtent2D vkSize,
        VkFormat vkFormat,
        VkRenderPass vkRenderPass,
        uint32_t layerCount);

    VkImage getVkImage() const { return mVkImage; }
    VkFramebuffer getVkFramebuffer() const { return mVkFramebuffer; };

private:
    VkDevice mVkDevice{};
    VkImage mVkImage{};
    VkImageView mVkImageView{};
    VkFramebuffer mVkFramebuffer{};
};
}