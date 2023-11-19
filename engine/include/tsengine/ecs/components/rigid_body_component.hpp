#pragma once

#include "tsengine/math.hpp"

namespace ts
{
inline namespace TS_VER
{
struct RigidBodyComponent : public Component
{
    float velocity;

    RigidBodyComponent(const float velocity_ = 1.f) : velocity{velocity_}
    {}
};
} // namespace ver
} // namespace ts