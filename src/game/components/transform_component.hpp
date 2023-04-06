#pragma once

#include "tsengine/math.hpp"

struct TransformComponent
{
    ts::Vec2 position;

    TransformComponent(ts::Vec2 position_ = {}) :
        position{ position_ }
    {}
};