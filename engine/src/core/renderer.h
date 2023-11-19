#pragma once

#include "internal_utils.h"
#include "tsengine/math.hpp"

#include "vulkan/vulkan.h"

namespace ts
{
inline namespace TS_VER
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
    TS_NOT_COPYABLE_AND_MOVEABLE(Renderer);

    static constexpr size_t framesInFlightCount{2};

public:
    Renderer(const Context& ctx, const Headset& headset);

    virtual ~Renderer();

    void createRenderer();
    void render(const size_t swapchainImageIndex);
    void submit(const bool useSemaphores) const;

    [[nodiscard]] VkSemaphore getCurrentDrawableSemaphore() const;
    [[nodiscard]] VkSemaphore getCurrentPresentableSemaphore() const;
    [[nodiscard]] VkCommandBuffer getCurrentCommandBuffer() const;

private:
    void createVertexIndexBuffer();
    void updateUniformData(const std::unique_ptr<RenderProcess>& renderProcess);
    void initRendererFrontend();

    const Context& mCtx;
    const Headset& mHeadset;
    VkCommandPool mCommandPool{};
    VkDescriptorPool mDescriptorPool{};
    VkDescriptorSetLayout mDescriptorSetLayout{};
    VkPipelineLayout mPipelineLayout{};
    std::array<std::unique_ptr<RenderProcess>, framesInFlightCount> mRenderProcesses{};
    std::shared_ptr<Pipeline> mGridPipeline, mNormalLightingPipeline, mPbrPipeline, mLightCubePipeline;
    size_t mIndexOffset{};
    std::unique_ptr<DataBuffer> mVertexIndexBuffer;
    size_t mCurrentRenderProcessIndex{};

};
} // namespace ver
} // namespace ts
