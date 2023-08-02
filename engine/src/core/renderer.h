#pragma once

#include "utils.hpp"
#include "tsengine/math.hpp"

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
    Renderer(const Context* ctx, const Headset* headset, const std::vector<Model*>& models, std::unique_ptr<MeshData>&& meshData);
    virtual ~Renderer();

    void createRenderer();
    void render(const math::Mat4& cameraMatrix, size_t swapchainImageIndex, float time);
    void submit(bool useSemaphores) const;

    [[nodiscard]] VkSemaphore getCurrentDrawableSemaphore() const;
    [[nodiscard]] VkSemaphore getCurrentPresentableSemaphore() const;
    [[nodiscard]] VkCommandBuffer getCurrentCommandBuffer() const;

private:
    void createVertexIndexBuffer();
    void updateUniformData(const math::Mat4& cameraMatrix, float time, RenderProcess* renderProcess);
    void beginRenderPass(
        VkCommandBuffer cmdBuffer,
        VkRenderPass pass,
        size_t currentImage,
        const VkRect2D area,
        VkFramebuffer fb = nullptr,
        uint32_t clearValueCount = 0,
        const VkClearValue* clearValues = nullptr);

    const Context* mCtx{};
    const Headset* mHeadset{};
    VkCommandPool mCommandPool{};
    VkDescriptorPool mDescriptorPool{};
    VkDescriptorSetLayout mDescriptorSetLayout{};
    VkPipelineLayout mPipelineLayout{};
    std::array<RenderProcess*, framesInFlightCount> mRenderProcesses{};
    Pipeline* mGridPipeline{}, *mDiffusePipeline{};
    size_t mIndexOffset{};
    DataBuffer* mVertexIndexBuffer{};
    size_t mCurrentRenderProcessIndex{};
    std::unique_ptr<MeshData> mMeshData;
    const std::vector<Model*>& mModels;
};
}
