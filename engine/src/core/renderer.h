#pragma once

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
    Renderer(const Context* ctx, const Headset* headset);
    ~Renderer();

    void createRenderer(std::unique_ptr<MeshData> meshData, const std::vector<Model*>& models);

private:
    void createVertexIndexBuffer(std::unique_ptr<MeshData>& meshData, const std::vector<Model*>& models);

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
};
}
