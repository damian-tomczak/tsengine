#pragma once

#include "internal_utils.h"
#include "tsengine/math.hpp"

#ifdef _WIN32
#define NOMINMAX
#endif // _WIN32

#include "vulkan/vulkan.h"

namespace ts
{
class AssetStore;
class Context;
struct Model;
class Headset;
class RenderProcess;
class Pipeline;
class DataBuffer;

class Renderer
{
    NOT_COPYABLE_AND_MOVEABLE(Renderer);

    static constexpr size_t framesInFlightCount{2};

public:
    Renderer(
        const Context& ctx,
        const Headset& headset,
        const AssetStore& assetStore);

    virtual ~Renderer();

    void createRenderer();
    void render(const math::Vec3& cameraPosition, const size_t swapchainImageIndex);
    void submit(const bool useSemaphores) const;

    [[nodiscard]] VkSemaphore getCurrentDrawableSemaphore() const;
    [[nodiscard]] VkSemaphore getCurrentPresentableSemaphore() const;
    [[nodiscard]] VkCommandBuffer getCurrentCommandBuffer() const;

private:
    void createVertexIndexBuffer();
    void updateUniformData(const math::Vec3& cameraMatrix, const std::unique_ptr<RenderProcess>& renderProcess);

    const Context& mCtx;
    const Headset& mHeadset;
    const AssetStore& mAssetStore;
    VkCommandPool mCommandPool{};
    VkDescriptorPool mDescriptorPool{};
    VkDescriptorSetLayout mDescriptorSetLayout{}, mLightCubeDescriptorSetLayout{};
    VkPipelineLayout mPipelineLayout{}, mLightCubeLayout{};
    std::array<std::unique_ptr<RenderProcess>, framesInFlightCount> mRenderProcesses{};
    std::unique_ptr<Pipeline> mGridPipeline, mNormalLightingPipeline, mPbrPipeline, mLightCubePipeline;
    size_t mIndexOffset{};
    std::unique_ptr<DataBuffer> mVertexIndexBuffer;
    size_t mCurrentRenderProcessIndex{};

};
}
