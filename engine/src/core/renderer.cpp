#include "renderer.h"
#include "context.h"
#include "game_object.hpp"
#include "vulkan_tools/vulkan_functions.h"
#include "renderer_process.h"
#include "pipeline.h"
#include "headset.h"
#include "data_buffer.h"
#include "khronos_utils.h"
#include "headset.h"
#include "render_target.h"

namespace ts
{
Renderer::Renderer(const Context& ctx, const Headset& headset, const std::vector<std::shared_ptr<Model>>& models, std::unique_ptr<MeshData>&& meshData) :
    mCtx{ctx},
    mHeadset{headset},
    mModels{models},
    mMeshData{std::move(meshData)}
{}

Renderer::~Renderer()
{
    delete mVertexIndexBuffer;
    delete mDiffusePipeline;
    delete mGridPipeline;

    const auto device = mCtx.getVkDevice();
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

    for (const auto renderProcess : mRenderProcesses)
    {
        delete renderProcess;
    }

    if ((device != nullptr) && (mCommandPool != nullptr))
    {
        vkDestroyCommandPool(device, mCommandPool, nullptr);
    }
}

void Renderer::createRenderer()
{
    const auto device = mCtx.getVkDevice();

    const VkCommandPoolCreateInfo commandPoolCreateInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = mCtx.getVkGraphicsQueueFamilyIndex()
    };
    LOGGER_VK(vkCreateCommandPool, device, &commandPoolCreateInfo, nullptr, &mCommandPool);

    const std::array descriptorPoolSizes{
        VkDescriptorPoolSize{
            .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = static_cast<uint32_t>(framesInFlightCount)
        },
    };

    const VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = static_cast<uint32_t>(framesInFlightCount),
        .poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size()),
        .pPoolSizes = descriptorPoolSizes.data(),
    };
    LOGGER_VK(vkCreateDescriptorPool, device, &descriptorPoolCreateInfo, nullptr, &mDescriptorPool);

    const std::array descriptorSetLayoutBindings{
        VkDescriptorSetLayoutBinding{
            .binding = 0,
            .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
            .descriptorCount = 1,
            .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        },
    };

    std::vector<VkPushConstantRange> pushConstantRanges;
    pushConstantRanges.emplace_back<VkPushConstantRange>({
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .offset = 0,
        .size = sizeof(Model::pos)
    });
    pushConstantRanges.emplace_back<VkPushConstantRange>({
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
        .offset = pushConstantRanges.size() > 0 ? pushConstantRanges.back().size : 0,
        .size = sizeof(Material::Params)
    });

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
        .pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size()),
        .pPushConstantRanges = pushConstantRanges.data(),
    };
    LOGGER_VK(vkCreatePipelineLayout, device, &pipelineLayoutCreateInfo, nullptr, &mPipelineLayout);

    for (auto& renderProcess : mRenderProcesses)
    {
        renderProcess = new RenderProcess{mCtx, mHeadset};
        renderProcess->createRendererProcess(mCommandPool, mDescriptorPool, mDescriptorSetLayout, mModels.size());
    }

    mGridPipeline = new Pipeline{mCtx};
    mGridPipeline->createPipeline(
        mPipelineLayout,
        mHeadset.getVkRenderPass(),
        "assets/shaders/grid.vert.spirv",
        "assets/shaders/grid.frag.spirv");

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

    mDiffusePipeline = new Pipeline{mCtx};
    mDiffusePipeline->createPipeline(
        mPipelineLayout,
        mHeadset.getVkRenderPass(),
        "assets/shaders/default.vert.spirv",
        "assets/shaders/default.frag.spirv",
        {vertexInputBindingDescription},
        {vertexInputAttributePosition, vertexInputAttributeNormal, vertexInputAttributeColor});

    createVertexIndexBuffer();

    mIndexOffset = mMeshData->getIndexOffset();
}

void Renderer::render(const math::Mat4 cameraMatrix, const size_t swapchainImageIndex)
{
    mCurrentRenderProcessIndex = (mCurrentRenderProcessIndex + 1) % mRenderProcesses.size();
    auto renderProcess = mRenderProcesses.at(mCurrentRenderProcessIndex);

    const auto busyFence = renderProcess->getFence();
    LOGGER_VK(vkWaitForFences, mCtx.getVkDevice(), 1, &busyFence, VK_TRUE, UINT64_MAX);
    LOGGER_VK(vkResetFences, mCtx.getVkDevice(), 1, &busyFence);

    const auto commandBuffer = renderProcess->getCommandBuffer();
    const VkCommandBufferBeginInfo commandBufferBeginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    LOGGER_VK(vkBeginCommandBuffer, commandBuffer, &commandBufferBeginInfo);

    updateUniformData(cameraMatrix, renderProcess);

    const std::array clearValues{
        VkClearValue{.color = {1.f, 1.f, 1.f, 1.f}},
        VkClearValue{.depthStencil = {1.0f, 0}}
    };

    VkRenderPassBeginInfo renderPassBeginInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = mHeadset.getVkRenderPass(),
        .framebuffer = mHeadset.getRenderTarget(swapchainImageIndex)->getFramebuffer(),
        .renderArea = {
            .extent = mHeadset.getEyeResolution(0),
        },
        .clearValueCount = static_cast<uint32_t>(clearValues.size()),
        .pClearValues = clearValues.data()
    };

    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{
        .x = static_cast<float>(renderPassBeginInfo.renderArea.offset.x),
        .y = static_cast<float>(renderPassBeginInfo.renderArea.offset.y),
        .width = static_cast<float>(renderPassBeginInfo.renderArea.extent.width),
        .height = static_cast<float>(renderPassBeginInfo.renderArea.extent.height),
        .minDepth = 0.f,
        .maxDepth = 1.f,
    };
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{
        .offset = renderPassBeginInfo.renderArea.offset,
        .extent = renderPassBeginInfo.renderArea.extent
    };
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkDeviceSize vertexOffset{};
    const auto buffer = mVertexIndexBuffer->getBuffer();
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &buffer, &vertexOffset);

    vkCmdBindIndexBuffer(commandBuffer, buffer, mIndexOffset, VK_INDEX_TYPE_UINT32);

    const auto descriptorSet = renderProcess->getDescriptorSet();
    // TODO: it beggs for the refactor
    for (size_t modelIdx{}; modelIdx < mModels.size() + 1; ++modelIdx)
    {
        vkCmdBindDescriptorSets(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            mPipelineLayout,
            0,
            1,
            &descriptorSet,
            0,
            nullptr);


        if (modelIdx >= 1)
        {
            mDiffusePipeline->bind(commandBuffer);
            vkCmdPushConstants(commandBuffer, mPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(math::Vec3), &mModels.at(modelIdx - 1)->pos);
        }
        else
        {
            mGridPipeline->bind(commandBuffer);
        }

        if (modelIdx != 0)
        {
            const auto model = mModels.at(modelIdx - 1);

            vkCmdDrawIndexed(
                commandBuffer,
                static_cast<uint32_t>(model->indexCount),
                1,
                static_cast<uint32_t>(model->firstIndex),
                0,
                0);
        }
        else
        {
            vkCmdDraw(commandBuffer, 6, 1, 0, 0);
        }
    }

    vkCmdEndRenderPass(commandBuffer);
}

void Renderer::submit(bool useSemaphores) const
{
    const auto renderProcess = mRenderProcesses.at(mCurrentRenderProcessIndex);
    const auto commandBuffer = renderProcess->getCommandBuffer();
    LOGGER_VK(vkEndCommandBuffer, commandBuffer);

    constexpr VkPipelineStageFlags waitStages{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    const auto drawableSemaphore = renderProcess->getDrawableSemaphore();
    const auto presentableSemaphore = renderProcess->getPresentableSemaphore();

    VkSubmitInfo submitInfo{
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = useSemaphores ? 1u : 0u,
        .pWaitSemaphores = useSemaphores ? &drawableSemaphore : nullptr,
        .pWaitDstStageMask = &waitStages,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
        .signalSemaphoreCount = useSemaphores ? 1u : 0u,
        .pSignalSemaphores = useSemaphores ? &presentableSemaphore : nullptr
    };

    LOGGER_VK(vkQueueSubmit, mCtx.getVkGraphicsQueue(), 1, &submitInfo, renderProcess->getFence());
}

VkSemaphore Renderer::getCurrentDrawableSemaphore() const
{
    return mRenderProcesses.at(mCurrentRenderProcessIndex)->getDrawableSemaphore();
}

VkSemaphore Renderer::getCurrentPresentableSemaphore() const
{
    return mRenderProcesses.at(mCurrentRenderProcessIndex)->getPresentableSemaphore();
}

VkCommandBuffer Renderer::getCurrentCommandBuffer() const
{
    return mRenderProcesses.at(mCurrentRenderProcessIndex)->getCommandBuffer();
}

void Renderer::createVertexIndexBuffer()
{
    const auto bufferSize = static_cast<VkDeviceSize>(mMeshData->getSize());
    auto stagingBuffer = new DataBuffer{mCtx};
    stagingBuffer->createDataBuffer(
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        bufferSize);

    auto bufferData = static_cast<char*>(stagingBuffer->map());
    if (!bufferData)
    {
        LOGGER_ERR("invalid mapping memory");
    }

    mMeshData->writeTo(bufferData);
    stagingBuffer->unmap();

    mVertexIndexBuffer = new DataBuffer{mCtx};
    mVertexIndexBuffer->createDataBuffer(
        VK_BUFFER_USAGE_TRANSFER_DST_BIT |
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        bufferSize);

    stagingBuffer->copyTo(*mVertexIndexBuffer, mRenderProcesses.at(0)->getCommandBuffer(), mCtx.getVkGraphicsQueue());

    delete stagingBuffer;
}

void Renderer::updateUniformData(const math::Mat4 cameraPos, RenderProcess* renderProcess)
{
    renderProcess->mStaticVertexUniformData.cameraPos = cameraPos;
    for (size_t eyeIndex{}; eyeIndex < mHeadset.getEyeCount(); ++eyeIndex)
    {
        renderProcess->mStaticVertexUniformData.viewMatrices.at(eyeIndex) = mHeadset.getEyeViewMatrix(eyeIndex);
        renderProcess->mStaticVertexUniformData.projectionMatrices.at(eyeIndex) = mHeadset.getEyeProjectionMatrix(eyeIndex);
    }

    renderProcess->updateUniformBufferData();
}
}
