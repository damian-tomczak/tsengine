#include "renderer_process.h"

#include "globals.hpp"

#include "context.h"
#include "vulkan_tools/vulkan_functions.h"
#include "tsengine/logger.h"
#include "khronos_utils.h"
#include "data_buffer.h"
#include "headset.h"

#include "tsengine/ecs/ecs.h"
#include "tsengine/ecs/components/renderer_component.hpp"
#include "ecs/systems/render_system.hpp"

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
    const VkCommandPool commandPool,
    const VkDescriptorPool descriptorPool,
    const VkDescriptorSetLayout descriptorSetLayout,
    const size_t modelsNum,
    const size_t lightsNum)
{
    mIndividualUniformData.resize(modelsNum);

    const auto device = mCtx.getVkDevice();

    const VkCommandBufferAllocateInfo commandBufferAllocateInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = commandPool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    TS_VK_CHECK(vkAllocateCommandBuffers, device, &commandBufferAllocateInfo, &mCommandBuffer);

    const VkSemaphoreCreateInfo semaphoreCreateInfo{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    TS_VK_CHECK(vkCreateSemaphore, device, &semaphoreCreateInfo, nullptr, &mDrawableSemaphore);

    TS_VK_CHECK(vkCreateSemaphore, device, &semaphoreCreateInfo, nullptr, &mPresentableSemaphore);

    const VkFenceCreateInfo fenceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT,
    };
    TS_VK_CHECK(vkCreateFence, device, &fenceCreateInfo, nullptr, &mFence);

    const auto uniformBufferOffsetAlignment = mCtx.getUniformBufferOffsetAlignment();

    std::vector<VkDescriptorBufferInfo> descriptorBufferInfos;
    descriptorBufferInfos.emplace_back(VkDescriptorBufferInfo{
        .offset = 0,
        .range = sizeof(decltype(mIndividualUniformData)::value_type),
    });

    descriptorBufferInfos.emplace_back(VkDescriptorBufferInfo{
        .offset = descriptorBufferInfos.at(0).offset +
            khronos_utils::align(descriptorBufferInfos.at(0).range, uniformBufferOffsetAlignment) *
            static_cast<VkDeviceSize>(modelsNum),
        .range = sizeof(mCommonUniformData),
    });

    descriptorBufferInfos.emplace_back(VkDescriptorBufferInfo{
        .offset = descriptorBufferInfos.at(1).offset +
            khronos_utils::align(descriptorBufferInfos.at(1).range, uniformBufferOffsetAlignment) *
            static_cast<VkDeviceSize>(lightsNum),
        .range = sizeof(decltype(mLightsUniformData.positions)) * mLightsUniformData.positions.size()
    });

    if (descriptorBufferInfos.empty())
    {
        TS_ERR("Workflow isn't yet prepared");
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
        TS_ERR("Invalid mapping memory");
    }

    const VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &descriptorSetLayout
    };
    TS_VK_CHECK(vkAllocateDescriptorSets, device, &descriptorSetAllocateInfo, &mDescriptorSet);

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
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
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
        TS_ERR("Uniform buffer wasn't allocated");
    }

    const auto uniformBufferOffsetAlignment = mCtx.getUniformBufferOffsetAlignment();

    auto offset = static_cast<int8_t*>(mUniformBufferMemory);

    {
        constexpr auto length = sizeof(decltype(mIndividualUniformData)::value_type);
        for (const auto& individualData : mIndividualUniformData)
        {
            memcpy(offset, &individualData, length);
            offset += khronos_utils::align(length, uniformBufferOffsetAlignment);
        }
    }

    {
        constexpr auto length = sizeof(decltype(mCommonUniformData));
        memcpy(offset, &mCommonUniformData, length);
        offset += khronos_utils::align(length, uniformBufferOffsetAlignment);
    }

    {
        constexpr auto length = sizeof(decltype(mLightsUniformData.positions));
        for (const auto& lightData : mLightsUniformData.positions)
        {
            memcpy(offset, &lightData, length);
            offset += khronos_utils::align(length, uniformBufferOffsetAlignment);
        }
    }
}
}
