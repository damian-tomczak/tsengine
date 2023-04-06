#pragma once

#include "tsengine/core.h"

#include "tsengine/event_bus.hpp"

struct KeyReleasedEvent : public ts::Event
{
    ts::Key symbol;
    KeyReleasedEvent(ts::Key symbol) : symbol(symbol) {}
};