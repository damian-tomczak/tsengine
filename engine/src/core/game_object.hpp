#pragma once

// ACHTUNG --- ONLY FOR DEVELOPMENT PURPOSES --- ACHTUNG
// TODO: refactor it to the ecs

#include "tsengine/math.hpp"

namespace ts
{
struct Model final
{
    size_t firstIndex;
    size_t indexCount;
    math::Matrix4x4<> worldMatrix;
};
}