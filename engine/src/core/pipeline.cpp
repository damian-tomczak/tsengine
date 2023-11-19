#include "pipeline.h"
#include "context.h"
#include "vulkan_tools/vulkan_functions.h"
#include "tsengine/logger.h"
#include "khronos_utils.h"


namespace ts
{
inline namespace TS_VER
{
namespace
{
void loadShaderFromFile(const VkDevice device, const std::string& fileName, VkShaderModule& shaderModule)
{
    std::ifstream file(fileName, std::ios::ate | std::ios::binary);
    if (!file.is_open())
    {
        TS_ERR(("Can not open shader file: " + fileName).c_str());
    }

    const auto fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> code(fileSize);
    file.seekg(0);
    file.read(code.data(), fileSize);
    file.close();

    VkShaderModuleCreateInfo shaderModuleCreateInfo{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = code.size(),
        .pCode = reinterpret_cast<const uint32_t*>(code.data())
    };

    TS_VK_CHECK(vkCreateShaderModule, device, &shaderModuleCreateInfo, nullptr, &shaderModule);
}
} // namespace

Pipeline::Pipeline(const Context& ctx) : mCtx{ctx}
{}

Pipeline::~Pipeline()
{
    const auto device = mCtx.getVkDevice();
    if ((device != nullptr) && (mPipeline != nullptr))
    {
        vkDestroyPipeline(device, mPipeline, nullptr);
    }
}

void Pipeline::createPipeline(
    VkPipelineLayout pipelinelineLayout,
    VkRenderPass renderPass,
    const std::string& vertexFilename,
    const std::string& fragmentFilename,
    const std::vector<VkVertexInputBindingDescription>& vertexInputBindingDescriptions,
    const std::vector<VkVertexInputAttributeDescription>& vertexInputAttributeDescriptions)
{
    const auto device = mCtx.getVkDevice();

    VkShaderModule vertexShaderModule;
    loadShaderFromFile(device, vertexFilename, vertexShaderModule);

    VkShaderModule fragmentShaderModule;
    loadShaderFromFile(device, fragmentFilename, fragmentShaderModule);

    const VkPipelineShaderStageCreateInfo pipelinelineShaderStageCreateInfoVertex{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertexShaderModule,
        .pName = "main"
    };

    const VkPipelineShaderStageCreateInfo pipelinelineShaderStageCreateInfoFragment{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragmentShaderModule,
        .pName = "main"
    };

    const std::array shaderStages{pipelinelineShaderStageCreateInfoVertex, pipelinelineShaderStageCreateInfoFragment};

    const VkPipelineVertexInputStateCreateInfo pipelinelineVertexInputStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindingDescriptions.size()),
        .pVertexBindingDescriptions = vertexInputBindingDescriptions.data(),
        .vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributeDescriptions.size()),
        .pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data()
    };

    const VkPipelineInputAssemblyStateCreateInfo pipelinelineInputAssemblyStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
    };

    const VkPipelineViewportStateCreateInfo pipelinelineViewportStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .viewportCount = 1,
        .scissorCount = 1
    };

    const VkPipelineRasterizationStateCreateInfo pipelinelineRasterizationStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
        .lineWidth = 1.f,
    };

    const VkPipelineMultisampleStateCreateInfo pipelinelineMultisampleStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = mCtx.getVkMultisampleCount()
    };

    const VkPipelineColorBlendAttachmentState pipelinelineColorBlendAttachmentState{
        .blendEnable = true,
        .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT,
    };

    const VkPipelineColorBlendStateCreateInfo pipelinelineColorBlendStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &pipelinelineColorBlendAttachmentState
    };

    constexpr std::array dynamicStates{
        VkDynamicState{VK_DYNAMIC_STATE_VIEWPORT},
        VkDynamicState{VK_DYNAMIC_STATE_SCISSOR}
    };

    const VkPipelineDynamicStateCreateInfo pipelinelineDynamicStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
        .pDynamicStates = dynamicStates.data()
    };

    const VkPipelineDepthStencilStateCreateInfo pipelinelineDepthStencilStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .depthTestEnable = true,
        .depthWriteEnable = true,
        .depthCompareOp = VK_COMPARE_OP_LESS,
    };

    const VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo{
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .stageCount = static_cast<uint32_t>(shaderStages.size()),
        .pStages = shaderStages.data(),
        .pVertexInputState = &pipelinelineVertexInputStateCreateInfo,
        .pInputAssemblyState = &pipelinelineInputAssemblyStateCreateInfo,
        .pViewportState = &pipelinelineViewportStateCreateInfo,
        .pRasterizationState = &pipelinelineRasterizationStateCreateInfo,
        .pMultisampleState = &pipelinelineMultisampleStateCreateInfo,
        .pDepthStencilState = &pipelinelineDepthStencilStateCreateInfo,
        .pColorBlendState = &pipelinelineColorBlendStateCreateInfo,
        .pDynamicState = &pipelinelineDynamicStateCreateInfo,
        .layout = pipelinelineLayout,
        .renderPass = renderPass,
    };
    TS_VK_CHECK(vkCreateGraphicsPipelines, device, nullptr, 1, &graphicsPipelineCreateInfo, nullptr, &mPipeline);

    vkDestroyShaderModule(device, vertexShaderModule, nullptr);
    vkDestroyShaderModule(device, fragmentShaderModule, nullptr);
}

void Pipeline::bind(const VkCommandBuffer commandBuffer) const
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);
}
} // namespace ver
} // namespace ts