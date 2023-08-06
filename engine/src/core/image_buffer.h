#pragma once

#include "utils.hpp"

#include "vulkan/vulkan.h"

namespace ts
{
class Context;

class ImageBuffer final
{
    NOT_COPYABLE_AND_MOVEABLE(ImageBuffer);

public:
    ImageBuffer(const Context& context) : mContext{context}
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
    const Context& mContext;
    VkImage mImage{};
    VkDeviceMemory mDeviceMemory{};
    VkImageView mImageView{};
};
}
