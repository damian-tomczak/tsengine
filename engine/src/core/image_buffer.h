#pragma once

#include "internal_utils.h"

#include "vulkan/vulkan.h"

namespace ts
{
class Context;

class ImageBuffer final
{
    TS_NOT_COPYABLE_AND_MOVEABLE(ImageBuffer);

public:
    ImageBuffer(const Context& ctx) : mCtx{ctx}
    {}
    ~ImageBuffer();

    void createImage(
        VkExtent2D size,
        VkFormat format,
        VkImageUsageFlagBits usage,
        VkSampleCountFlagBits samples,
        VkImageAspectFlags aspect,
        size_t layerCount);

    [[nodiscard]] VkImageView getVkImageView() const { return mImageView; }

private:
    const Context& mCtx;
    VkImage mImage{};
    VkDeviceMemory mDeviceMemory{};
    VkImageView mImageView{};
};
}
