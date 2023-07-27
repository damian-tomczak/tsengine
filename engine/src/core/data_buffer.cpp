#include "data_buffer.h"
#include "vulkan_tools/vulkan_functions.h"
#include "context.h"
#include "tsengine/logger.h"
#include "khronos_utils.hpp"

namespace ts
{
DataBuffer::DataBuffer(const Context* ctx) : mCtx{ctx}
{}

DataBuffer::~DataBuffer()
{
    const auto device{mCtx->getVkDevice()};
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

void DataBuffer::createDataBuffer(VkBufferUsageFlags bufferUsageFlags, VkMemoryPropertyFlags memoryProperties, VkDeviceSize size)
{
    mSize = size;

    const auto device{mCtx->getVkDevice()};

    VkBufferCreateInfo bufferCreateInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = bufferUsageFlags,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE
    };
    LOGGER_VK(vkCreateBuffer, device, &bufferCreateInfo, nullptr, &mBuffer);

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, mBuffer, &memoryRequirements);

    uint32_t suitableMemoryTypeIndex = 0u;
    if (!khronos_utils::findSuitableMemoryTypeIndex(mCtx->getVkPhysicalDevice(), memoryRequirements, memoryProperties,
        suitableMemoryTypeIndex))
    {
        LOGGER_ERR("can not find the suitable memory index");
    }

    VkMemoryAllocateInfo memoryAllocateInfo{
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = suitableMemoryTypeIndex
    };
    LOGGER_VK(vkAllocateMemory, device, &memoryAllocateInfo, nullptr, &mDeviceMemory);

    LOGGER_VK(vkBindBufferMemory, device, mBuffer, mDeviceMemory, 0);
}

void DataBuffer::copyTo(const DataBuffer& target, VkCommandBuffer commandBuffer, VkQueue queue) const
{
    VkCommandBufferBeginInfo beginInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
    };
    LOGGER_VK(vkBeginCommandBuffer, commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{
        .size = mSize
    };
    vkCmdCopyBuffer(commandBuffer, mBuffer, target.getBuffer(), 1, &copyRegion);

    LOGGER_VK(vkEndCommandBuffer, commandBuffer);

    VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer
    };
    LOGGER_VK(vkQueueSubmit, queue, 1, &submitInfo, VK_NULL_HANDLE);

    LOGGER_VK(vkQueueWaitIdle, queue);
}

void* DataBuffer::map() const
{
    void* data;
    LOGGER_VK(vkMapMemory, mCtx->getVkDevice(), mDeviceMemory, 0, mSize, 0, &data);

    return data;
}

void DataBuffer::unmap() const
{
    vkUnmapMemory(mCtx->getVkDevice(), mDeviceMemory);
}
}