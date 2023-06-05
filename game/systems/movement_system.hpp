
#include "tsengine/ecs.hpp"

#include "components/rigid_body_component.hpp"
#include "components/transform_component.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>

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
        (void)deltaTime;

        for (auto entity : getSystemEntities())
        {
            auto& transformComponent = entity.getComponent<TransformComponent>();
            auto& rigidbodyComponent = entity.getComponent<RigidBodyComponent>();
            (void)transformComponent;
            (void)rigidbodyComponent;

            if (entity.hasTag("player"))
            {
            }
        }
    }
};
