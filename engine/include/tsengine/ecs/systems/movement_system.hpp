#pragma once

#include "tsengine/ecs/ecs.hpp"

#include "tsengine/ecs/components/transform_component.hpp"
#include "tsengine/ecs/components/rigid_body_component.hpp"

namespace ts
{
class MovementSystem : public ts::System
{
public:
    MovementSystem()
    {
        requireComponent<TransformComponent>();
        requireComponent<RigidBodyComponent>();
    }

    void update(const float deltaTime)
    {

    }
};
}
