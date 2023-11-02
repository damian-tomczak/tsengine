#pragma once

#include "tsengine/ecs/ecs.hpp"

#include "tsengine/ecs/components/mesh_component.hpp"
#include "tsengine/ecs/components/transform_component.hpp"
#include "tsengine/ecs/components/renderer_component.hpp"

#include "core/renderer_process.h"
#include "core/pipeline.h"
#include "khronos_utils.h"

#include "shaders/light_cube.h"
#include "shaders/grid.h"

#include "vulkan_tools/vulkan_functions.h"

namespace ts
{
class Renderer;

class RenderSystem : public System
{
public:
    RenderSystem(const VkDeviceSize vkUniformBufferOffsetAlignment) :
        mVkUniformBufferOffsetAlignment{vkUniformBufferOffsetAlignment}
    {
        requireComponent<RendererComponentBase>();

        gRegistry.addSystem<Lights>();
    }

    void update(const VkCommandBuffer cmdBuf, const VkDescriptorSet descriptorSet)
    {
        for (const auto entity : getSystemEntities())
        {
            size_t entityIndexforUniformBufferOffset{};
            const auto uniformBufferOffset = static_cast<uint32_t>(
                khronos_utils::align(
                    static_cast<VkDeviceSize>(sizeof(decltype(RenderProcess::mIndividualUniformData)::value_type)),
                    mVkUniformBufferOffsetAlignment) * static_cast<VkDeviceSize>(entityIndexforUniformBufferOffset));

            vkCmdBindDescriptorSets(
                cmdBuf,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                mpPipelineLayout,
                0,
                1,
                &descriptorSet,
                1,
                &uniformBufferOffset);

            if (!entity.hasComponent<TransformComponent>())
            {
                vkCmdPushConstants(cmdBuf,
                    mpPipelineLayout,
                    VK_SHADER_STAGE_VERTEX_BIT,
                    0,
                    sizeof(math::Vec3),
                    &entity.getComponent<TransformComponent>().pos);
            }

            if (entity.hasComponent<MeshComponent>())
            {
                TS_ASSERT(!entity.hasComponent<RendererComponent<PipelineType::COLOR>>(), "Not implemented yet");

                const auto& mesh = entity.getComponent<MeshComponent>();

                if (entity.hasComponent<RendererComponent<PipelineType::NORMAL_LIGHTING>>())
                {
                    if (const auto pipe = mpNormalLightingPipeline.lock())
                    {
                        pipe->bind(cmdBuf);
                    }
                    else
                    {
                        throw Exception{ "Invalid normal lighting pipeline" };
                    }
                }
                else if (entity.hasComponent<RendererComponent<PipelineType::PBR>>())
                {
                    if (const auto pipe = mpPbrPipeline.lock())
                    {
                        pipe->bind(cmdBuf);
                    }
                    else
                    {
                        throw Exception{ "Invalid pbr pipeline" };
                    }
                
                    vkCmdPushConstants(cmdBuf,
                        mpPipelineLayout,
                        VK_SHADER_STAGE_FRAGMENT_BIT,
                        sizeof(math::Vec3),
                        sizeof(RendererComponent<PipelineType::PBR>::Material),
                        &entity.getComponent<RendererComponent<PipelineType::PBR>>().material);
                }

                vkCmdDrawIndexed(cmdBuf,
                    static_cast<uint32_t>(mesh.indexCount),
                    1,
                    static_cast<uint32_t>(mesh.firstIndex),
                    0,
                    0);
            }
            else if (entity.hasComponent<RendererComponent<PipelineType::LIGHT>>())
            {
                if (const auto pipe = mpLightCubePipeline.lock())
                {
                    pipe->bind(cmdBuf);
                }
                else
                {
                    throw Exception{"Invalid pbr pipeline"};
                }

                vkCmdDraw(cmdBuf, LIGHT_CUBE_DRAW_CALL_VERTEX_COUNT, 1, 0, 0);
            }
            else if (entity.hasComponent<RendererComponent<PipelineType::GRID>>())
            {
                if (auto pipe = mpGridPipeline.lock())
                {
                    pipe->bind(cmdBuf);
                }
                else
                {
                    throw Exception{"Invalid grid pipeline"};
                }

                vkCmdDraw(cmdBuf, GRID_DRAW_CALL_VERTEX_COUNT, 1, 0, 0);
            }
            else
            {
                TS_ERR("Unexpected rendering workflow");
            }
        }
    }

    class Lights : public System
    {
    public:
        Lights()
        {
            requireComponent<RendererComponent<PipelineType::LIGHT>>();
        }

    };

private:
    friend Renderer;
    const VkDeviceSize& mVkUniformBufferOffsetAlignment;
    std::weak_ptr<Pipeline> mpGridPipeline, mpNormalLightingPipeline, mpPbrPipeline, mpLightCubePipeline;
    VkPipelineLayout mpPipelineLayout{};
};
}