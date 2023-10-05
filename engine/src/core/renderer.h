#pragma once

#include "internal_utils.h"
#include "tsengine/math.hpp"

#ifdef _WIN32
#define NOMINMAX
#endif // _WIN32

#include "vulkan/vulkan.h"

namespace ts
{
class MeshData;
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
    Renderer(const Context& ctx, const Headset& headset, const std::vector<std::shared_ptr<Model>>& models, std::unique_ptr<MeshData>&& meshData);
    virtual ~Renderer();

    void createRenderer();
    void render(const math::Vec3& cameraPosition, size_t swapchainImageIndex);
    void submit(bool useSemaphores) const;

    [[nodiscard]] VkSemaphore getCurrentDrawableSemaphore() const;
    [[nodiscard]] VkSemaphore getCurrentPresentableSemaphore() const;
    [[nodiscard]] VkCommandBuffer getCurrentCommandBuffer() const;

private:
    void createVertexIndexBuffer();
    void updateUniformData(const math::Vec3& cameraMatrix, const std::unique_ptr<RenderProcess>& renderProcess);

    const Context& mCtx;
    const Headset& mHeadset;
    VkCommandPool mCommandPool{};
    VkDescriptorPool mDescriptorPool{};
    VkDescriptorSetLayout mDescriptorSetLayout{};
    VkPipelineLayout mPipelineLayout{};
    std::array<std::unique_ptr<RenderProcess>, framesInFlightCount> mRenderProcesses{};
    std::unique_ptr<Pipeline> mGridPipeline, mNormalLightingPipeline, mPbrPipeline;
    size_t mIndexOffset{};
    std::unique_ptr<DataBuffer> mVertexIndexBuffer;
    size_t mCurrentRenderProcessIndex{};
    std::unique_ptr<MeshData> mMeshData;
    const std::vector<std::shared_ptr<Model>>& mModels;
};
}
