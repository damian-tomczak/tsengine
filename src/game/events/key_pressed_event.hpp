#pragma once

#include "tsengine/core.h"
#include "tsengine/event_bus.hpp"

struct KeyPressedEvent : public ts::Event
{
    ts::Key symbol;
    KeyPressedEvent(ts::Key symbol) : symbol(symbol) {}
};