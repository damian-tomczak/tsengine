#pragma once

#include "tsengine/math.hpp"

#include <vulkan/vulkan.h>

namespace ts
{
class Context;
class DataBuffer;

class RenderProcess final
{
public:
    RenderProcess(const Context* ctx) : mCtx{ ctx }
    {}
    ~RenderProcess();

    void createRendererProcess(
        VkCommandPool commandPool,
        VkDescriptorPool descriptorPool,
        VkDescriptorSetLayout descriptorSetLayout,
        size_t modelCount);

    struct DynamicVertexUniformData
    {
        math::Mat4 worldMatrix;
    };
    std::vector<DynamicVertexUniformData> mDynamicVertexUniformData;

    struct StaticVertexUniformData
    {
        std::array<math::Mat4, 2> viewProjectionMatrices;
    } mStaticVertexUniformData;

    struct StaticFragmentUniformData
    {
        float time;
    } mStaticFragmentUniformData;

    VkCommandBuffer getCommandBuffer() const { return mCommandBuffer; }
    VkFence getFence() const { return mFence; }
    VkDescriptorSet getDescriptorSet() const { return mDescriptorSet; }
    void updateUniformBufferData() const;

private:
    const Context* mCtx{};
    VkCommandBuffer mCommandBuffer{};
    VkSemaphore mDrawableSemaphore{}, mPresentableSemaphore{};
    VkFence mFence{};
    DataBuffer* mUniformBuffer{};
    void* mUniformBufferMemory{};
    VkDescriptorSet mDescriptorSet{};
};
}