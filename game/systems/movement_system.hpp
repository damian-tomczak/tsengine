#pragma once

#include "tsengine/ecs.hpp"

#include "components/rigid_body_component.hpp"
#include "components/transform_component.hpp"

class MovementSystem : public ts::System
{
public:
    MovementSystem()
    {}

    void update(const float deltaTime)
    {}
};
