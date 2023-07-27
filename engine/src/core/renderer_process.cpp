#include "renderer_process.h"
#include "context.h"
#include "vulkan_tools/vulkan_functions.h"
#include "tsengine/logger.h"
#include "khronos_utils.hpp"
#include "data_buffer.h"

namespace ts
{
RenderProcess::RenderProcess(const Context* ctx) : mCtx{ctx}
{}

RenderProcess::~RenderProcess()
{
    if (mUniformBuffer != nullptr)
    {
        mUniformBuffer->unmap();
    }
    delete mUniformBuffer;

    const auto device{mCtx->getVkDevice()};
    if (device)
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
    dynamicVertexUniformData.resize(modelCount);
    for (size_t modelIndex{}; modelIndex < modelCount; ++modelIndex)
    {
        dynamicVertexUniformData.at(modelIndex).worldMatrix = math::Mat4::makeScalarMat(1.f);
    }

    for (auto& viewProjectionMatrix : staticVertexUniformData.viewProjectionMatrices)
    {
        viewProjectionMatrix = math::Mat4::makeScalarMat(1.f);
    }

    staticFragmentUniformData.time = 0.0f;

    const auto device{mCtx->getVkDevice()};

    const VkCommandBufferAllocateInfo commandBufferAllocateInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    LOGGER_VK(vkAllocateCommandBuffers, device, &commandBufferAllocateInfo, &mCommandBuffer);

    const VkSemaphoreCreateInfo semaphoreCreateInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
    LOGGER_VK(vkCreateSemaphore, device, &semaphoreCreateInfo, nullptr, &mDrawableSemaphore);

    LOGGER_VK(vkCreateSemaphore, device, &semaphoreCreateInfo, nullptr, &mPresentableSemaphore);

    const VkFenceCreateInfo fenceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };
    LOGGER_VK(vkCreateFence, device, &fenceCreateInfo, nullptr, &mFence);

    const auto uniformBufferOffsetAlignment{mCtx->getUniformBufferOffsetAlignment()};

    std::array<VkDescriptorBufferInfo, 3> descriptorBufferInfos;
    descriptorBufferInfos.at(0).offset = 0u;
    descriptorBufferInfos.at(0).range = sizeof(DynamicVertexUniformData);

    descriptorBufferInfos.at(1).offset =
        khronos_utils::align(descriptorBufferInfos.at(0u).range, uniformBufferOffsetAlignment) * static_cast<VkDeviceSize>(modelCount);
    descriptorBufferInfos.at(1).range = sizeof(StaticVertexUniformData);

    descriptorBufferInfos.at(2).offset =
        descriptorBufferInfos.at(1).offset + khronos_utils::align(descriptorBufferInfos.at(1u).range, uniformBufferOffsetAlignment);
    descriptorBufferInfos.at(2).range = sizeof(StaticFragmentUniformData);

    const auto uniformBufferSize{descriptorBufferInfos.at(2).offset + descriptorBufferInfos.at(2).range};
    mUniformBuffer = new DataBuffer{mCtx};
    mUniformBuffer->createDataBuffer(
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        uniformBufferSize);

    mUniformBufferMemory = mUniformBuffer->map();
    if (!mUniformBufferMemory)
    {
        LOGGER_ERR("invalid mapping memory");
    }

    const VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &descriptorSetLayout
    };
    LOGGER_VK(vkAllocateDescriptorSets, device, &descriptorSetAllocateInfo, &mDescriptorSet);

    for (VkDescriptorBufferInfo& descriptorBufferInfo : descriptorBufferInfos)
    {
        descriptorBufferInfo.buffer = mUniformBuffer->getBuffer();
    }

    std::array<VkWriteDescriptorSet, 3u> writeDescriptorSets{{
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = mDescriptorSet,
            .dstBinding = 0,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
            .pBufferInfo = &descriptorBufferInfos.at(0),
        },
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = mDescriptorSet,
            .dstBinding = 1,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &descriptorBufferInfos.at(1),
        },
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = mDescriptorSet,
            .dstBinding = 2,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &descriptorBufferInfos.at(2)
    }}};

    vkUpdateDescriptorSets(
        device,
        static_cast<uint32_t>(writeDescriptorSets.size()),
        writeDescriptorSets.data(), 0,
        nullptr);
}
}
