#pragma once

#include "vulkan/vulkan.h"
#include "utils.hpp"

namespace ts
{
class RenderTarget final
{
    NOT_COPYABLE_AND_MOVEABLE(RenderTarget);

public:
    RenderTarget(VkDevice device, VkImage image) : mDevice(device), mImage(image)
    {}
    ~RenderTarget();

    void createRenderTarget(
        VkImageView colorImageView,
        VkImageView depthImageView,
        VkExtent2D size,
        VkFormat format,
        VkRenderPass renderPass,
        uint32_t layerCount);

    VkImage getVkImage() const { return mImage; }
    VkFramebuffer getVkFramebuffer() const { return mFramebuffer; };

private:
    VkDevice mDevice{};
    VkImage mImage{};
    VkImageView mImageView{};
    VkFramebuffer mFramebuffer{};
};
}