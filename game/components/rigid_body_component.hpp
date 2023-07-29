#pragma once

#include "tsengine/math.hpp"

struct RigidBodyComponent
{
    ts::math::Vec2 velocity;

    RigidBodyComponent(ts::math::Vec2 velocity_ = {}) : velocity{ velocity_ }
    {
    }
};
