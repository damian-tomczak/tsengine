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
    {}

    void subscribeToEvents(std::unique_ptr<ts::EventBus>& pEventBus)
    {}

    void onKeyPressed(KeyPressedEvent& event)
    {}

    void onKeyReleased(KeyReleasedEvent& event)
    {}
};
