
#include "tsengine/ecs.hpp"

#include "components/transform_component.hpp"
#include "components/rigid_body_component.hpp"

#include <iostream>
#include <cmath>
#include <algorithm>

class MovementSystem : public ts::System
{
public:
    MovementSystem()
    {
        requireComponent<TransformComponent>();
        requireComponent<RigidBodyComponent>();
    }

    void update(float deltaTime)
    {
        for (auto entity : getSystemEntities())
        {
            auto& transformComponent = entity.getComponent<TransformComponent>();
            auto& rigidbodyComponent = entity.getComponent<RigidBodyComponent>();

            if (entity.hasTag("player"))
            {
            }
        }
    }
};
