#pragma once

#include "tsengine/math.hpp"

struct TransformComponent
{
    ts::math::Vec2 position;

    TransformComponent(ts::math::Vec2 position_ = {}) : position{ position_ }
    {
    }
};