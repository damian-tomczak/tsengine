#include "renderer_process.h"
#include "context.h"
#include "vulkan_tools/vulkan_functions.h"
#include "tsengine/logger.h"
#include "khronos_utils.h"
#include "data_buffer.h"
#include "headset.h"

namespace ts
{
RenderProcess::RenderProcess(const Context& ctx, const Headset& headset) : mCtx{ctx}, mHeadset{headset}
{}

RenderProcess::~RenderProcess()
{
    if (mUniformBuffer != nullptr)
    {
        mUniformBuffer->unmap();
    }
    delete mUniformBuffer;

    const auto device = mCtx.getVkDevice();
    if (device != nullptr)
    {
        if (mFence != nullptr)
        {
            vkDestroyFence(device, mFence, nullptr);
        }

        if (mPresentableSemaphore != nullptr)
        {
            vkDestroySemaphore(device, mPresentableSemaphore, nullptr);
        }

        if (mDrawableSemaphore != nullptr)
        {
            vkDestroySemaphore(device, mDrawableSemaphore, nullptr);
        }
    }
}

void RenderProcess::createRendererProcess(
    VkCommandPool commandPool,
    VkDescriptorPool descriptorPool,
    VkDescriptorSetLayout descriptorSetLayout,
    size_t modelCount)
{
    const auto device = mCtx.getVkDevice();

    const VkCommandBufferAllocateInfo commandBufferAllocateInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    LOGGER_VK(vkAllocateCommandBuffers, device, &commandBufferAllocateInfo, &mCommandBuffer);

    const VkSemaphoreCreateInfo semaphoreCreateInfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    LOGGER_VK(vkCreateSemaphore, device, &semaphoreCreateInfo, nullptr, &mDrawableSemaphore);

    LOGGER_VK(vkCreateSemaphore, device, &semaphoreCreateInfo, nullptr, &mPresentableSemaphore);

    const VkFenceCreateInfo fenceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };
    LOGGER_VK(vkCreateFence, device, &fenceCreateInfo, nullptr, &mFence);

    const auto uniformBufferOffsetAlignment = mCtx.getUniformBufferOffsetAlignment();

    std::array descriptorBufferInfos{
        VkDescriptorBufferInfo{
            .offset = 0,
            .range = sizeof(mStaticVertexUniformData)
        },
    };

    const auto uniformBufferSize = descriptorBufferInfos.back().offset + descriptorBufferInfos.back().range;
    mUniformBuffer = new DataBuffer{mCtx};
    mUniformBuffer->createDataBuffer(
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        uniformBufferSize);

    mUniformBufferMemory = mUniformBuffer->map();
    if (!mUniformBufferMemory)
    {
        LOGGER_ERR("Invalid mapping memory");
    }

    const VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &descriptorSetLayout
    };
    LOGGER_VK(vkAllocateDescriptorSets, device, &descriptorSetAllocateInfo, &mDescriptorSet);

    for (auto& descriptorBufferInfo : descriptorBufferInfos)
    {
        descriptorBufferInfo.buffer = mUniformBuffer->getBuffer();
    }

    std::array writeDescriptorSets{
        VkWriteDescriptorSet{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = mDescriptorSet,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &descriptorBufferInfos.at(0),
        },
    };

    vkUpdateDescriptorSets(
        device,
        static_cast<uint32_t>(writeDescriptorSets.size()),
        writeDescriptorSets.data(),
        0,
        nullptr);
}

void RenderProcess::updateUniformBufferData() const
{
    if (mUniformBufferMemory == nullptr)
    {
        return;
    }

    const auto uniformBufferOffsetAlignment = mCtx.getUniformBufferOffsetAlignment();

    auto offset = static_cast<char*>(mUniformBufferMemory);

    VkDeviceSize length = sizeof(mStaticVertexUniformData.cameraPos);
    memcpy(offset, &mStaticVertexUniformData.cameraPos, length);
    offset += khronos_utils::align(length, uniformBufferOffsetAlignment);

    length = sizeof(math::Mat4);
    for (const auto& viewMatrix : mStaticVertexUniformData.viewMatrices)
    {
        memcpy(offset, &viewMatrix, length);
        offset += khronos_utils::align(length, uniformBufferOffsetAlignment);
    }

    for (const auto& projectionMatrix : mStaticVertexUniformData.projectionMatrices)
    {
        memcpy(offset, &projectionMatrix, length);
        offset += khronos_utils::align(length, uniformBufferOffsetAlignment);
    }
}
}
