#pragma once

#include "tsengine/math.hpp"

namespace ts
{
struct RigidBodyComponent : public Component
{
    float velocity;

    RigidBodyComponent(const float velocity_ = 1.f) : velocity{velocity_}
    {}
};
}