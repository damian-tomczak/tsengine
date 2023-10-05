#pragma once

#include "tsengine/math.hpp"
#include "internal_utils.h"

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

    struct IndivialData final
    {
        math::Mat4 model;
    };
    std::vector<IndivialData> mIndividualUniformData;

    struct CommonUniformData final
    {
        math::Vec3 cameraPosition;
        std::array<math::Mat4, 2> viewMatrices;
        std::array<math::Mat4, 2> projMat;
    } mCommonUniformData;

    // TODO: deffered lighting
    // TODO: move it to the different place
    struct LightData final
    {
        std::array<math::Vec3, 2> lightPositions{
            math::Vec3{0.f, 5.f, -5.f},
            math::Vec3{0.f, 5.f, 5.f},
        };
    } mLightUniformData;

    void updateUniformBufferData();

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
    std::unique_ptr<DataBuffer> mUniformBuffer;
    void* mUniformBufferMemory;
    VkDescriptorSet mDescriptorSet{};
    const Headset& mHeadset;
};
}