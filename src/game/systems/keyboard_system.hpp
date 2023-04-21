#pragma once

#include "tsengine/ecs.hpp"
#include "tsengine/event_bus.hpp"

#include "events/key_pressed_event.hpp"
#include "events/key_released_event.hpp"

#include "components/rigid_body_component.hpp"
#include "components/texture_component.hpp"
#include "components/transform_component.hpp"

class KeyboardSystem : public ts::System
{
public:
    KeyboardSystem()
    {
        requireComponent<TransformComponent>();
        requireComponent<RigidBodyComponent>();
        requireComponent<TextureComponent>();
    }

    void subscribeToEvents(std::unique_ptr<ts::EventBus>& pEventBus)
    {
        pEventBus->subscribeToEvent<KeyPressedEvent>(this, &KeyboardSystem::onKeyPressed);
        pEventBus->subscribeToEvent<KeyReleasedEvent>(this, &KeyboardSystem::onKeyReleased);
    }

    void onKeyPressed(KeyPressedEvent& event)
    {
        for (auto entity : getSystemEntities())
        {
            auto& transformComponent{ entity.getComponent<TransformComponent>() };
            auto& rigidBodyComponent{ entity.getComponent<RigidBodyComponent>() };
            auto& spriteComponent{ entity.getComponent<TextureComponent>() };

            if (entity.hasTag("player"))
            {
            }
        }
    }

    void onKeyReleased(KeyReleasedEvent& event)
    {
        for (auto entity : getSystemEntities())
        {
            auto& rigidBodyComponent{ entity.getComponent<RigidBodyComponent>() };
            auto& transformComponent{ entity.getComponent<TransformComponent>() };

            if (entity.hasTag("player"))
            {
            }
        }
    }
};
