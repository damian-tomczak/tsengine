#pragma once

namespace ts
{
struct Vec2
{
    float x;
    float y;

    Vec2 operator*(float scalar) const { return Vec2{x * scalar, y * scalar}; }
};

struct Matrix4x4
{
    std::array<std::array<float, 4>, 4> mData;
};
} // namespace ts