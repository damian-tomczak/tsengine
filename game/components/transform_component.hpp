#pragma once

#include "tsengine/math.hpp"

using namespace ts::math;

struct TransformComponent
{
    Vec2 position;

    TransformComponent(Vec2 position_) : position{position_}
    {
    }
};