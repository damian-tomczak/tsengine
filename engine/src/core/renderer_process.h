#pragma once

#include "tsengine/math.hpp"
#include "utils.hpp"

#include "vulkan/vulkan.h"

namespace ts
{
class Context;
class DataBuffer;
class Headset;

class RenderProcess final
{
    NOT_COPYABLE_AND_MOVEABLE(RenderProcess);

public:
    RenderProcess(const Context& ctx, const Headset& headset);
    ~RenderProcess();

    void createRendererProcess(
        VkCommandPool commandPool,
        VkDescriptorPool descriptorPool,
        VkDescriptorSetLayout descriptorSetLayout,
        size_t modelCount);

    struct StaticVertexUniformData
    {
        math::Mat4 cameraPos;
        std::array<math::Mat4, 2> viewMatrices;
        std::array<math::Mat4, 2> projectionMatrices;
    } mStaticVertexUniformData;

    void updateUniformBufferData() const;

    [[nodiscard]] VkCommandBuffer getCommandBuffer() const { return mCommandBuffer; }
    [[nodiscard]] VkFence getFence() const { return mFence; }
    [[nodiscard]] VkDescriptorSet getDescriptorSet() const { return mDescriptorSet; }
    [[nodiscard]] VkSemaphore getDrawableSemaphore() const { return mDrawableSemaphore; }
    [[nodiscard]] VkSemaphore getPresentableSemaphore() const { return mPresentableSemaphore; }

private:
    const Context& mCtx;
    VkCommandBuffer mCommandBuffer{};
    VkSemaphore mDrawableSemaphore{}, mPresentableSemaphore{};
    VkFence mFence{};
    DataBuffer* mUniformBuffer{};
    void* mUniformBufferMemory{};
    VkDescriptorSet mDescriptorSet{};
    const Headset& mHeadset;
};
}