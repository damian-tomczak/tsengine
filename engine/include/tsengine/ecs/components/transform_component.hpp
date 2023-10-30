#pragma once

#include "tsengine/math.hpp"

namespace ts
{
struct TransformComponent
{
    math::Vec3 pos;
    math::Mat4 modelMat;
};
}
