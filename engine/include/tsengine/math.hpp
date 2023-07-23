#pragma once

/**
 * Mathematical solutions grow with the needs of the codebase.
 * Header is slightly inspired by glm, but isn't so optimized
 */

namespace ts::math
{
template <typename T = float>
struct Vec2
{
    T x;
    T y;

    template<typename T>
    Vec2 operator*(T scalar) const { return {x * scalar, y * scalar}; }
};

template <typename T = float>
struct Vec3
{
    T x;
    T y;
    T z;
};

template <typename T = float>
struct Matrix4x4
{
private:
    using Row = std::array<T, 4>;

public:
    std::array<Row, 4> data;

    static Matrix4x4<T> makeScalarMat(const T value)
    {
        Matrix4x4<T> result;
        result[0][0] = value;
        result[1][1] = value;
        result[2][2] = value;
        result[3][3] = value;
        return result;
    }

    Row& operator[](const int32_t index)
    {
        return data[index];
    }
};

template <typename T = float>
inline Matrix4x4<T> translate(Matrix4x4<T> matrix, Vec3<T> translation)
{
    matrix[3][0] += translation.x;
    matrix[3][1] += translation.y;
    matrix[3][2] += translation.z;

    return matrix;
}

template<typename ParameterT, typename ReturnT = float>
inline ReturnT radians(ParameterT degrees)
{
    constexpr auto factor{std::numbers::pi / 180};
    return degrees * factor;
}
} // namespace ts