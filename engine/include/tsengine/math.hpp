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
    template <typename Z>
    using Row = std::array<Z, 4>;

public:
    Row<Row<T>> data;

    static Matrix4x4<T> makeScalarMat(const T value)
    {
        return {
        {{
            value, 0    , 0    , 0    ,
            0    , value, 0    , 0    ,
            0    , 0    , value, 0    ,
            0    , 0    , 0    , value
        }}};
    }

    Row<T>& operator[](const int32_t index)
    {
        return data[index];
    }

    const std::array<T, 4>& operator[](const int32_t index) const
    {
        return data[index];
    }
};

template <typename T = float, typename Z = float>
inline Matrix4x4<T> translate(const Matrix4x4<T>& matrix, Vec3<Z> translation)
{
    return {
    {{
        matrix[0][0]                , matrix[0][1]                , matrix[0][2]                , matrix[0][3],
        matrix[1][0]                , matrix[1][1]                , matrix[1][2]                , matrix[1][3],
        matrix[2][0]                , matrix[2][1]                , matrix[2][2]                , matrix[2][3],
        matrix[3][0] + translation.x, matrix[3][1] + translation.y, matrix[3][2] + translation.z, matrix[3][3]
    }}};
}

template<typename ParameterT, typename ReturnT = float>
inline ReturnT radians(ParameterT degrees)
{
    constexpr auto factor{std::numbers::pi / 180};
    return degrees * factor;
}
} // namespace ts