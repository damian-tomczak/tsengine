#pragma once

#include "tsengine/math.hpp"

struct RigidBodyComponent
{
    ts::Vec2 velocity;

    RigidBodyComponent(ts::Vec2 velocity_ = {}) : velocity{ velocity_ }
    {
    }
};
