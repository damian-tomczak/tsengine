#pragma once

#include "tsengine/math.hpp"

#include "utils.hpp"
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
    ~Renderer();

    void createRenderer();
    void render(const math::Mat4& cameraMat, size_t swapchainImageIndex, float time);

private:
    void createVertexIndexBuffer();
    void updateUniformData(const math::Mat4& cameraMat, float time, RenderProcess* renderProcess);

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
