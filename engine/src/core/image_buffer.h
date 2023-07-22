#pragma once

#include "context.h"

namespace ts
{
class ImageBuffer final
{
    NOT_COPYABLE_AND_MOVEABLE(ImageBuffer);

public:
    ImageBuffer(const Context& ctx) : mCtx(ctx)
    {}
    ~ImageBuffer();

    void createImage(
        VkExtent2D vkSize,
        VkFormat vkFormat,
        VkImageUsageFlagBits vkUsage,
        VkSampleCountFlagBits vkSamples,
        VkImageAspectFlags vkAspect,
        size_t layerCount);

    VkImageView getVkImageView() const { return mVkImageView; }

private:
    const Context& mCtx;
    VkImage mVkImage{};
    VkDeviceMemory mVkDeviceMemory{};
    VkImageView mVkImageView{};
};
}
