#pragma once

#include "tsengine/ecs/ecs.hpp"

#include "tsengine/ecs/components/mesh_component.hpp"

class RenderSystem : public ts::System
{
public:
    RenderSystem()
    {}

    void update(const float deltaTime)
    {
        (void)deltaTime;
    }
};