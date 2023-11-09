#pragma once

#include "tsengine/math.hpp"

namespace ts
{
struct TransformComponent : public Component
{
    math::Vec3 pos;
    math::Mat4 modelMat{1.f};

    TransformComponent(const math::Vec3 pos_ = math::Vec3{0.f}) : pos{pos_}
    {}
};
}
