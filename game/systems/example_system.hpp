#pragma once

#include "tsengine/logger.h"

#include "tsengine/ecs/ecs.h"

#include "tsengine/ecs/components/transform_component.hpp"
#include "components/example_component.hpp"
#include "components/echo_component.hpp"

#include <utility>

class ExampleSystem : public ts::System
{
    inline static constexpr std::chrono::seconds spheresMovementDuration{2s};

public:
    ExampleSystem()
    {
        requireComponent<ts::TransformComponent>();
    }

    void update()
    {
        for (const auto entity : getSystemEntities())
        {
            if (entity.hasComponent<EchoComponent>())
            {
                const auto& echoComponent = entity.getComponent<EchoComponent>();

                if (mWelcomedEntities.contains(echoComponent.messageId))
                {
                    TS_LOG(std::format(R"(Entity "{}" says "{}")",
                        entity.getTag(),
                        echoComponent.message).c_str());

                    mWelcomedEntities.insert(echoComponent.messageId);
                }

            }

            if (entity.hasComponent<ExampleComponent>())
            {
                auto& exampleComponent = entity.getComponent<ExampleComponent>();

                if (entity.belongsToGroup("spheres"))
                {
                    const auto currentTime = std::chrono::high_resolution_clock::now();
                    const auto elapsedTime = currentTime - exampleComponent.startTime;

                    const auto newPos = ts::math::lerp(
                        exampleComponent.startPos,
                        exampleComponent.endPos,
                        elapsedTime / std::chrono::duration<float>(spheresMovementDuration));

                    entity.getComponent<ts::TransformComponent>().pos = newPos;

                    if (elapsedTime >= std::chrono::duration<float>(spheresMovementDuration))
                    {
                        exampleComponent.startTime = currentTime;

                        std::swap(exampleComponent.startPos, exampleComponent.endPos);
                    }
                }
            }
        }
    }

    std::set<size_t> mWelcomedEntities;
};
