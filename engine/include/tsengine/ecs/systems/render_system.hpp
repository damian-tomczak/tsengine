#pragma once

#include "tsengine/ecs/ecs.hpp"

#include "tsengine/ecs/components/mesh_component.hpp"
#include "tsengine/ecs/components/transform_component.hpp"
#include "tsengine/ecs/components/renderer_component.hpp"

namespace ts
{
class RenderSystem : public ts::System
{
public:
    RenderSystem()
    {
        requireComponent<ts::TransformComponent>();
#define PIPELINE(type) requireComponent<ts::RendererComponent<PipelineType::type>>();
        TS_PIPELINES_LIST
#undef PIPELINE
    }

    void update(const float deltaTime)
    {
        (void)deltaTime;
    }
};
}