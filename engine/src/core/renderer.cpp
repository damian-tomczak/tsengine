#include "renderer.h"
#include "context.h"
#include "game_object.hpp"
#include "vulkan_tools/vulkan_functions.h"
#include "renderer_process.h"
#include "pipeline.h"
#include "headset.h"
#include "data_buffer.h"

namespace ts
{
Renderer::Renderer(const Context* ctx, const Headset* headset) : mCtx{ctx}, mHeadset{headset}
{}

void Renderer::createRenderer(std::unique_ptr<MeshData> meshData, const std::vector<Model*>& models)
{
    const auto device{mCtx->getVkDevice()};

    const VkCommandPoolCreateInfo commandPoolCreateInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = mCtx->getVkGraphicsQueueFamilyIndex()
    };
    LOGGER_VK(vkCreateCommandPool, device, &commandPoolCreateInfo, nullptr, &mCommandPool);

    const std::array<VkDescriptorPoolSize, 2> descriptorPoolSizes{ {
        {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
            .descriptorCount = static_cast<uint32_t>(framesInFlightCount),
        },
        {
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = static_cast<uint32_t>(framesInFlightCount * 2)
        }
    } };

    const VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = static_cast<uint32_t>(framesInFlightCount),
        .poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size()),
        .pPoolSizes = descriptorPoolSizes.data(),
    };
    LOGGER_VK(vkCreateDescriptorPool, device, &descriptorPoolCreateInfo, nullptr, &mDescriptorPool);

    const std::array<VkDescriptorSetLayoutBinding, 3> descriptorSetLayoutBindings{{
        {
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        },
        {
            .binding = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        },
        {
            .binding = 2,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        }
    }};

    const VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = static_cast<uint32_t>(descriptorSetLayoutBindings.size()),
        .pBindings = descriptorSetLayoutBindings.data()
    };
    LOGGER_VK(vkCreateDescriptorSetLayout, device, &descriptorSetLayoutCreateInfo, nullptr, &mDescriptorSetLayout);

    const VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = 1,
        .pSetLayouts = &mDescriptorSetLayout,
    };
    LOGGER_VK(vkCreatePipelineLayout, device, &pipelineLayoutCreateInfo, nullptr, &mPipelineLayout);

    for (auto& renderProcess : mRenderProcesses)
    {
        renderProcess = new RenderProcess(mCtx);
        renderProcess->createRendererProcess(mCommandPool, mDescriptorPool, mDescriptorSetLayout, models.size());
    }

    const VkVertexInputBindingDescription vertexInputBindingDescription{
        .binding = 0,
        .stride = sizeof(Vertex),
        .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
    };

    const VkVertexInputAttributeDescription vertexInputAttributePosition{
        .location = 0,
        .binding = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(Vertex, position)
    };

    const VkVertexInputAttributeDescription vertexInputAttributeNormal{
        .location = 1,
        .binding = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(Vertex, normal),
    };

    const VkVertexInputAttributeDescription vertexInputAttributeColor{
        .location = 2,
        .binding = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(Vertex, color),
    };

    mGridPipeline = new Pipeline{mCtx};
    mGridPipeline->createPipeline(
        mPipelineLayout,
        mHeadset->getVkRenderPass(),
        "assets/shaders/grid.vert.spirv",
        "assets/shaders/grid.frag.spirv",
        {vertexInputBindingDescription},
        {vertexInputAttributePosition, vertexInputAttributeColor});

    mDiffusePipeline = new Pipeline{mCtx};
    mDiffusePipeline->createPipeline(
        mPipelineLayout,
        mHeadset->getVkRenderPass(),
        "assets/shaders/diffuse.vert.spirv",
        "assets/shaders/diffuse.frag.spirv",
        {vertexInputBindingDescription},
        {vertexInputAttributePosition, vertexInputAttributeNormal, vertexInputAttributeColor});

    createVertexIndexBuffer(meshData, models);

    mIndexOffset = meshData->getIndexOffset();
}

void Renderer::createVertexIndexBuffer(std::unique_ptr<MeshData>& meshData, const std::vector<Model*>& models)
{
    const auto bufferSize{static_cast<VkDeviceSize>(meshData->getSize())};
    auto stagingBuffer{new DataBuffer{mCtx}};
    stagingBuffer->createDataBuffer(
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        bufferSize);

    char* bufferData{static_cast<char*>(stagingBuffer->map())};
    if (!bufferData)
    {
        LOGGER_ERR("invalid mapping memory");
    }

    meshData->writeTo(bufferData);
    stagingBuffer->unmap();

    mVertexIndexBuffer = new DataBuffer{mCtx};
    mVertexIndexBuffer->createDataBuffer(
        VK_BUFFER_USAGE_TRANSFER_DST_BIT |
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        bufferSize);

    stagingBuffer->copyTo(*mVertexIndexBuffer, mRenderProcesses.at(0)->getCommandBuffer(), mCtx->getVkGraphicsQueue());

    delete stagingBuffer;
}

Renderer::~Renderer()
{
    delete mVertexIndexBuffer;
    delete mDiffusePipeline;
    delete mGridPipeline;

    const VkDevice device{mCtx->getVkDevice()};
    if (device != nullptr)
    {
        if (mPipelineLayout != nullptr)
        {
            vkDestroyPipelineLayout(device, mPipelineLayout, nullptr);
        }

        if (mDescriptorSetLayout != nullptr)
        {
            vkDestroyDescriptorSetLayout(device, mDescriptorSetLayout, nullptr);
        }

        if (mDescriptorPool != nullptr)
        {
            vkDestroyDescriptorPool(device, mDescriptorPool, nullptr);
        }
    }

    for (const RenderProcess* renderProcess : mRenderProcesses)
    {
        delete renderProcess;
    }

    if ((device != nullptr) && (mCommandPool != nullptr))
    {
        vkDestroyCommandPool(device, mCommandPool, nullptr);
    }
}

}
