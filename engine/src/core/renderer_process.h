#pragma once

#include "tsengine/math.hpp"
#include "internal_utils.h"
#include "shaders/common.h"

#include "vulkan/vulkan.h"

namespace ts
{
inline namespace TS_VER
{
class Context;
class DataBuffer;
class Headset;

class RenderProcess final
{
    TS_NOT_COPYABLE_AND_MOVEABLE(RenderProcess);

public:
    RenderProcess(const Context& ctx, const Headset& headset);
    ~RenderProcess();

    void createRendererProcess(
        const VkCommandPool commandPool,
        const VkDescriptorPool descriptorPool,
        const VkDescriptorSetLayout descriptorSetLayout,
        const size_t modelsNum,
        const size_t lightsNum);

    struct IndivialData final
    {
        math::Mat4 model;
    };
    std::vector<IndivialData> mIndividualUniformData{};
    
    struct LightData final
    {
        std::array<math::Vec3, LIGHTS_N> positions;
    } mLightsUniformData{};

    struct CommonUniformData final
    {
        math::Vec3 cameraPosition;
        std::array<math::Mat4, 2> viewMats;
        std::array<math::Mat4, 2> projMats;
    } mCommonUniformData{};

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
    void* mUniformBufferMemory{};
    VkDescriptorSet mDescriptorSet{};
    const Headset& mHeadset;
};
} // namespace ver
} // namespace ts