#pragma once

#include "tsengine/math.hpp"

namespace ts
{
struct RigidBodyComponent
{
    float velocity;

    RigidBodyComponent(const float velocity = 1.f) : velocity{velocity}
    {}
};
}