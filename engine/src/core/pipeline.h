#pragma once

#include <vulkan/vulkan.h>

namespace ts
{
class Context;

class Pipeline final
{
public:
    Pipeline(const Context& ctx);
    ~Pipeline();

    void createPipeline(
        VkPipelineLayout pipelineLayout,
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