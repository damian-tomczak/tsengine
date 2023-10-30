#pragma once

#include "tsengine/ecs/ecs.hpp"

#include "tsengine/ecs/components/rigid_body_component.hpp"
#include "tsengine/ecs/components/transform_component.hpp"

class MovementSystem : public ts::System
{
public:
    MovementSystem()
    {}

    void update(const float deltaTime)
    {}
};
