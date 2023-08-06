#pragma once

#include "utils.hpp"

#include "vulkan/vulkan.h"

namespace ts
{
class Context;

class DataBuffer final
{
    NOT_COPYABLE_AND_MOVEABLE(DataBuffer);

public:
    DataBuffer(const Context& context);
    ~DataBuffer();

    void createDataBuffer(VkBufferUsageFlags bufferUsageFlags, VkMemoryPropertyFlags memoryProperties, VkDeviceSize size);

    [[nodiscard]] VkBuffer getBuffer() const { return mBuffer; }

    void copyTo(const DataBuffer& target, VkCommandBuffer commandBuffer, VkQueue queue) const;
    void* map() const;
    void unmap() const;

private:
    const Context& mContext;
    VkBuffer mBuffer{};
    VkDeviceMemory mDeviceMemory{};
    VkDeviceSize mSize{};
};
}