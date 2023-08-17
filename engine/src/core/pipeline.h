#pragma once

#include "utils.hpp"
#include "vulkan/vulkan.h"

namespace ts
{
class Context;

class Pipeline final
{
    NOT_COPYABLE_AND_MOVEABLE(Pipeline);

public:
    Pipeline(const Context& ctx);
    ~Pipeline();

    void createPipeline(
        VkPipelineLayout pipelinelineLayout,
        VkRenderPass renderPass,
        const std::string& vertexFilename,
        const std::string& fragmentFilename,
        const std::vector<VkVertexInputBindingDescription>& vertexInputBindingDescriptions = {},
        const std::vector<VkVertexInputAttributeDescription>& vertexInputAttributeDescriptions = {});

    void bind(const VkCommandBuffer commandBuffer) const;

private:
    const Context& mCtx;
    VkPipeline mPipeline{};
};
}