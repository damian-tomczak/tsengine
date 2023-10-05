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
    mUniformBuffer.reset();

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
    mIndividualUniformData.resize(modelCount);

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

    std::vector<VkDescriptorBufferInfo> descriptorBufferInfos;
    descriptorBufferInfos.emplace_back(VkDescriptorBufferInfo{
        .offset = 0,
        .range = sizeof(decltype(mIndividualUniformData)::value_type),
    });

    descriptorBufferInfos.emplace_back(VkDescriptorBufferInfo{
        .offset = descriptorBufferInfos.at(0).offset + khronos_utils::align(descriptorBufferInfos.at(0).range, uniformBufferOffsetAlignment) * static_cast<VkDeviceSize>(modelCount),
        .range = sizeof(mCommonUniformData),
    });

    descriptorBufferInfos.emplace_back(VkDescriptorBufferInfo{
        .offset = descriptorBufferInfos.at(1).offset + khronos_utils::align(descriptorBufferInfos.at(1).range, uniformBufferOffsetAlignment),
        .range = sizeof(mLightUniformData),
    });

    if (descriptorBufferInfos.empty())
    {
        LOGGER_ERR("path not yet prepared");
    }

    const auto uniformBufferSize = descriptorBufferInfos.back().offset + descriptorBufferInfos.back().range;
    mUniformBuffer = std::make_unique<DataBuffer>(mCtx);
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
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
            .pBufferInfo = &descriptorBufferInfos.at(0),
        },
        VkWriteDescriptorSet{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = mDescriptorSet,
            .dstBinding = 1,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .pBufferInfo = &descriptorBufferInfos.at(1),
        },
        VkWriteDescriptorSet{
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = mDescriptorSet,
            .dstBinding = 2,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, // TODO: SBBO
            .pBufferInfo = &descriptorBufferInfos.at(2),
        },
    };

    vkUpdateDescriptorSets(
        device,
        static_cast<uint32_t>(writeDescriptorSets.size()),
        writeDescriptorSets.data(),
        0,
        nullptr);
}

void RenderProcess::updateUniformBufferData()
{
    if (mUniformBufferMemory == nullptr)
    {
        LOGGER_ERR("Uniform buffer wasn't allocated");
    }

    const auto uniformBufferOffsetAlignment = mCtx.getUniformBufferOffsetAlignment();

    auto offset = static_cast<char*>(mUniformBufferMemory);

    VkDeviceSize length{sizeof(decltype(mIndividualUniformData)::value_type)};
    for (const auto& individualData : mIndividualUniformData)
    {
        memcpy(offset, &individualData, length);
        offset += khronos_utils::align(length, uniformBufferOffsetAlignment);
    }

    length = sizeof(decltype(mCommonUniformData));
    memcpy(offset, &mCommonUniformData, length);
    offset += khronos_utils::align(length, uniformBufferOffsetAlignment);

    length = sizeof(mLightUniformData);
    memcpy(offset, &mLightUniformData, length);
    offset += khronos_utils::align(length, uniformBufferOffsetAlignment);
}
}
