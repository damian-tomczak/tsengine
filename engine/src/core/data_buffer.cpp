#include "data_buffer.h"
#include "vulkan_tools/vulkan_functions.h"
#include "context.h"
#include "tsengine/logger.h"
#include "khronos_utils.h"

namespace ts
{
inline namespace TS_VER
{
DataBuffer::DataBuffer(const Context& ctx) : mCtx{ctx}
{}

DataBuffer::~DataBuffer()
{
    const auto device = mCtx.getVkDevice();
    if (device != nullptr)
    {
        if (mDeviceMemory != nullptr)
        {
            vkFreeMemory(device, mDeviceMemory, nullptr);
        }

        if (mBuffer != nullptr)
        {
            vkDestroyBuffer(device, mBuffer, nullptr);
        }
    }
}

void DataBuffer::createDataBuffer(const VkBufferUsageFlags bufferUsageFlags, const VkMemoryPropertyFlags memoryProperties, const VkDeviceSize size)
{
    mSize = size;

    const auto device = mCtx.getVkDevice();

    VkBufferCreateInfo bufferCreateInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = bufferUsageFlags,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };
    TS_VK_CHECK(vkCreateBuffer, device, &bufferCreateInfo, nullptr, &mBuffer);

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, mBuffer, &memoryRequirements);

    uint32_t suitableMemoryTypeIndex{0};
    if (!khronos_utils::findSuitableMemoryTypeIndex(mCtx.getVkPhysicalDevice(), memoryRequirements, memoryProperties,
        suitableMemoryTypeIndex))
    {
        TS_ERR("Can not find the suitable memory index");
    }

    VkMemoryAllocateInfo memoryAllocateInfo{
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = suitableMemoryTypeIndex
    };
    TS_VK_CHECK(vkAllocateMemory, device, &memoryAllocateInfo, nullptr, &mDeviceMemory);

    TS_VK_CHECK(vkBindBufferMemory, device, mBuffer, mDeviceMemory, 0);
}

void DataBuffer::copyTo(const DataBuffer& target, VkCommandBuffer commandBuffer, VkQueue queue) const
{
    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };
    TS_VK_CHECK(vkBeginCommandBuffer, commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{
        .size = mSize
    };
    vkCmdCopyBuffer(commandBuffer, mBuffer, target.getBuffer(), 1, &copyRegion);

    TS_VK_CHECK(vkEndCommandBuffer, commandBuffer);

    VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer
    };
    TS_VK_CHECK(vkQueueSubmit, queue, 1, &submitInfo, VK_NULL_HANDLE);

    TS_VK_CHECK(vkQueueWaitIdle, queue);
}

void* DataBuffer::map() const
{
    void* data;
    TS_VK_CHECK(vkMapMemory, mCtx.getVkDevice(), mDeviceMemory, 0, mSize, 0, &data);

    return data;
}

void DataBuffer::unmap() const
{
    vkUnmapMemory(mCtx.getVkDevice(), mDeviceMemory);
}
} // namespace ver
} // namespace ts