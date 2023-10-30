#pragma once

#include "tsengine/ecs/ecs.hpp"
#include "tsengine/event_bus.hpp"

#include "events/key_pressed_event.hpp"
#include "events/key_released_event.hpp"

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
