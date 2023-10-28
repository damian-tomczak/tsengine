#pragma once

#include "utils.hpp"

// TODO: column major
namespace ts::math
{
struct Vec2;
struct Vec3;
struct Vec4;
struct Mat2;
struct Mat3;
struct Mat4;
struct Quat;

struct Vec2 final
{
    float x{}, y{};

    constexpr Vec2() = default;
    constexpr Vec2(const float v);
    constexpr Vec2(const Vec3& vec3);
    constexpr Vec2(const Vec4& vec4);
    constexpr Vec2(const float x_, const float y_);

    [[nodiscard]] constexpr Vec2 operator*(const float scalar) const { return {x * scalar, y * scalar}; }
    [[nodiscard]] constexpr Vec2 operator+(const Vec2& rhs) const { return {x + rhs.x, y * rhs.y}; }
    constexpr Vec2& operator+=(const Vec2& rhs);
    constexpr auto operator<=>(const Vec2& other) const = default;

    bool isNan() const { return std::isnan(x) && std::isnan(y); }
    bool isInf() const { return std::isinf(x) && std::isinf(y); }
};

inline Vec2 normalize(const Vec2 vec);

struct alignas(16) Vec3 final
{
    float x{}, y{}, z{};

    constexpr Vec3() = default;
    constexpr Vec3(const float v);
    constexpr Vec3(const Vec4& vec4);
    constexpr Vec3(const float x_, const float y_, const float z_);
    [[nodiscard]] constexpr Vec3 operator*(const float scalar) const { return {x * scalar, y * scalar, z * scalar}; }
    [[nodiscard]] constexpr Vec3 operator+(const Vec3& rhs) const { return {x + rhs.x, y * rhs.y, z + rhs.z}; }
    constexpr Vec3& operator+=(const Vec3& rhs);
    constexpr auto operator<=>(const Vec3& other) const = default;

    bool isNan() const { return std::isnan(x) && std::isnan(y) && std::isnan(z); }
    bool isInf() const { return std::isinf(x) && std::isinf(y) && std::isinf(z); }
};

inline Vec3 normalize(const Vec3& vec);

struct Vec4 final
{
    float x{}, y{}, z{}, w{};

    constexpr Vec4() = default;
    constexpr Vec4(const float v);
    constexpr Vec4(const float x_, const float y_, const float z_, const float w_);
    [[nodiscard]] constexpr Vec4 operator*(const float scalar) const { return {x * scalar, y * scalar, z * scalar, w * scalar}; }
    [[nodiscard]] constexpr Vec4 operator+(const Vec4& rhs) const { return {x + rhs.x, y * rhs.y, z + rhs.z, w + rhs.w}; }
    constexpr Vec4& operator+=(const Vec4& rhs);
    constexpr auto operator<=>(const Vec4& other) const = default;

    bool isNan() const { return std::isnan(x) or std::isnan(y) or std::isnan(z) or std::isnan(w); }
    bool isInf() const { return std::isinf(x) or std::isinf(y) or std::isinf(z) or std::isinf(w); }
};

inline Vec4 normalize(const Vec4& vec);

struct Mat2
{
    std::array<Vec2, 2> data{};

    constexpr Mat2();
    constexpr Mat2(
        const float x1, const float y1,
        const float x2, const float y2);

    constexpr Mat2(const float value);

    Vec2& operator[](const size_t index);
    const Vec2& operator[](const size_t index) const;
};

inline constexpr Mat2 operator*(const Mat2& lhs, const Mat2& rhs);
inline constexpr Mat2 transpose(const Mat2& mat);
inline Mat2 inverse(const Mat2& mat);
inline std::string to_string(const Mat2& mat);

struct Mat3
{
    std::array<Vec3, 3> data{};

    constexpr Mat3();
    constexpr Mat3(
        const float x1, const float y1, const float z1,
        const float x2, const float y2, const float z2,
        const float x3, const float y3, const float z3);

    constexpr Mat3(const float value);
    constexpr Mat3(const Quat& quat);

    Vec3& operator[](const size_t index);
    const Vec3& operator[](const size_t index) const;
};

inline constexpr Mat3 operator*(const Mat3& lhs, const Mat3& rhs);
inline constexpr Mat3 transpose(const Mat3& mat);
inline constexpr Mat3 translate(const Mat3& matrix, Vec2 translation);
inline Mat3 inverse(const Mat3& mat);
inline std::string to_string(const Mat3& mat);

struct Mat4
{
    std::array<Vec4, 4> data{};

    constexpr Mat4();
    constexpr Mat4(
        const float x1, const float y1, const float z1, const float w1,
        const float x2, const float y2, const float z2, const float w2,
        const float x3, const float y3, const float z3, const float w3,
        const float x4, const float y4, const float z4, const float w4);

    constexpr Mat4(const float v);
    constexpr Mat4(const Quat& quat);

    Vec4& operator[](const size_t index);
    const Vec4& operator[](const size_t index) const;
};

inline constexpr Mat4 operator*(const Mat4& lhs, const Mat4& rhs);
inline constexpr Mat4 transpose(const Mat4& mat);
inline constexpr Mat4 translate(const Mat4& matrix, const Vec3& translation);
inline constexpr Mat4 scale(const Mat4& matrix, const Vec3& scaleVec);
inline std::string to_string(const Mat4 mat);

struct Quat
{
    float w{}, x{}, y{}, z{};
};

inline constexpr Vec2::Vec2(const float v) : x{v}, y{v} {}
inline constexpr Vec2::Vec2(const Vec3& vec3) : x{vec3.x}, y{vec3.y} {}
inline constexpr Vec2::Vec2(const Vec4& vec4) : x{vec4.x}, y{vec4.y} {}
inline constexpr Vec2::Vec2(const float x_, const float y_) : x{x_}, y{y_} {}

template<typename T>
inline auto radians(T degrees)
{
    constexpr auto factor = std::numbers::pi_v<T> / 180;
    return degrees * factor;
}

inline Mat4 inverse(const Mat4& mat)
{
    const auto det23zw = mat[2].z * mat[3].w - mat[3].z * mat[2].w;
    const auto det23yw = mat[2].y * mat[3].w - mat[3].y * mat[2].w;
    const auto det23yz = mat[2].y * mat[3].z - mat[3].y * mat[2].z;
    const auto det23xw = mat[2].x * mat[3].w - mat[3].x * mat[2].w;
    const auto det23xz = mat[2].x * mat[3].z - mat[3].x * mat[2].z;
    const auto det23xy = mat[2].x * mat[3].y - mat[3].x * mat[2].y;
    const auto det13zw = mat[1].z * mat[3].w - mat[3].z * mat[1].w;
    const auto det13yw = mat[1].y * mat[3].w - mat[3].y * mat[1].w;
    const auto det13yz = mat[1].y * mat[3].z - mat[3].y * mat[1].z;
    const auto det13xw = mat[1].x * mat[3].w - mat[3].x * mat[1].w;
    const auto det13xz = mat[1].x * mat[3].z - mat[3].x * mat[1].z;
    const auto det13xy = mat[1].x * mat[3].y - mat[3].x * mat[1].y;
    const auto det12zw = mat[1].z * mat[2].w - mat[2].z * mat[1].w;
    const auto det12yw = mat[1].y * mat[2].w - mat[2].y * mat[1].w;
    const auto det12yz = mat[1].y * mat[2].z - mat[2].y * mat[1].z;
    const auto det12xw = mat[1].x * mat[2].w - mat[2].x * mat[1].w;
    const auto det12xz = mat[1].x * mat[2].z - mat[2].x * mat[1].z;
    const auto det12xy = mat[1].x * mat[2].y - mat[2].x * mat[1].y;

    const auto tempInverse00 = +(mat[1].y * det23zw - mat[1].z * det23yw + mat[1].w * det23yz);
    const auto tempInverse01 = -(mat[1].x * det23zw - mat[1].z * det23xw + mat[1].w * det23xz);
    const auto tempInverse02 = +(mat[1].x * det23yw - mat[1].y * det23xw + mat[1].w * det23xy);
    const auto tempInverse03 = -(mat[1].x * det23yz - mat[1].y * det23xz + mat[1].z * det23xy);

    const auto det =
        mat[0].x * tempInverse00 +
        mat[0].y * tempInverse01 +
        mat[0].z * tempInverse02 +
        mat[0].w * tempInverse03;

    if (det == 0)
    {
        throw std::runtime_error{"Singular matrix, can't find its inversion."};
    }

    return
    {
        tempInverse00 / det, (-(mat[0].y * det23zw - mat[0].z * det23yw + mat[0].w * det23yz)) / det, (+(mat[0].y * det13zw - mat[0].z * det13yw + mat[0].w * det13yz)) / det, (-(mat[0].y * det12zw - mat[0].z * det12yw + mat[0].w * det12yz)) / det,
        tempInverse01 / det, (+(mat[0].x * det23zw - mat[0].z * det23xw + mat[0].w * det23xz)) / det, (-(mat[0].x * det13zw - mat[0].z * det13xw + mat[0].w * det13xz)) / det, (+(mat[0].x * det12zw - mat[0].z * det12xw + mat[0].w * det12xz)) / det,
        tempInverse02 / det, (-(mat[0].x * det23yw - mat[0].y * det23xw + mat[0].w * det23xy)) / det, (+(mat[0].x * det13yw - mat[0].y * det13xw + mat[0].w * det13xy)) / det, (-(mat[0].x * det12yw - mat[0].y * det12xw + mat[0].w * det12xy)) / det,
        tempInverse03 / det, (+(mat[0].x * det23yz - mat[0].y * det23xz + mat[0].z * det23xy)) / det, (-(mat[0].x * det13yz - mat[0].y * det13xz + mat[0].z * det13xy)) / det, (+(mat[0].x * det12yz - mat[0].y * det12xz + mat[0].z * det12xy)) / det,
    };
}

inline constexpr Vec2& Vec2::operator+=(const Vec2& rhs)
{
    x += rhs.x;
    y += rhs.y;
    return *this;
}

inline Vec2 normalize(const Vec2 vec)
{
    const auto mag = std::sqrt(vec.x * vec.x + vec.y * vec.y);
    return {vec.x / mag, vec.y / mag};
}

inline constexpr Vec3::Vec3(const float v) : x{ v }, y{ v }, z{ v } {}
inline constexpr Vec3::Vec3(const Vec4& vec4) : x{ vec4.x }, y{ vec4.y }, z{ vec4.z } {}
inline constexpr Vec3::Vec3(const float x_, const float y_, const float z_) : x{ x_ }, y{ y_ }, z{ z_ } {}

inline constexpr Vec3& Vec3::operator+=(const Vec3& rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

inline Vec3 normalize(const Vec3& vec)
{
    const auto mag = std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    return {vec.x / mag, vec.y / mag, vec.z / mag};
}

inline constexpr Vec4& Vec4::operator+=(const Vec4& rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    w += rhs.w;
    return *this;
}

inline Vec4 normalize(const Vec4& vec)
{
    const auto mag = std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w);
    return {vec.x / mag, vec.y / mag, vec.z / mag, vec.w / mag};
}

inline constexpr Mat2::Mat2()
{}

inline constexpr Mat2::Mat2(
    const float x1, const float y1,
    const float x2, const float y2)
    :
    data
    {{
        {x1, y1},
        {x2, y2}
    }}
{}

inline constexpr Mat2::Mat2(const float value) :
    data
{{
    {value,   0.f},
    {  0.f, value}
}}
{}

inline Vec2& Mat2::operator[](const size_t index)
{
    if (index > 2)
    {
        throw std::runtime_error("invalid index specified");
    }

    return data[index];
}

inline const Vec2& Mat2::operator[](const size_t index) const
{
    if (index > 2)
    {
        throw std::runtime_error("invalid index specified");
    }

    return data[index];
}

inline constexpr Mat2 operator*(const Mat2& lhs, const Mat2& rhs)
{
    return
    {
        rhs.data[0].x * lhs.data[0].x + rhs.data[0].y * lhs.data[1].x,
        rhs.data[0].x * lhs.data[0].y + rhs.data[0].y * lhs.data[1].y,

        rhs.data[1].x * lhs.data[0].x + rhs.data[1].y * lhs.data[1].x,
        rhs.data[1].x * lhs.data[0].y + rhs.data[1].y * lhs.data[1].y,
    };
}

inline constexpr Mat2 transpose(const Mat2& mat)
{
    return
    {
        mat.data[0].x, mat.data[1].x,
        mat.data[0].y, mat.data[1].y,
    };
}

inline Mat2 inverse(const Mat2& mat)
{
    const auto det = mat[0].x * mat[1].y - mat[0].y * mat[1].x;

    if (det == 0)
    {
        throw std::runtime_error{ "singular matrix, can't find its inverse" };
    }
    return
    {
       (+mat[1].y) / det, (-mat[0].y) / det,
       (-mat[1].x) / det, (+mat[0].x) / det,
    };
}

inline std::string to_string(const Mat2& mat)
{
    return std::format("\n"
        "{:>10.7f}, {:>10.7f},\n"
        "{:>10.7f}, {:>10.7f}\n",
        mat[0].x, mat[0].y,
        mat[1].x, mat[1].y);
}

inline constexpr Mat3::Mat3()
{}

inline constexpr Mat3::Mat3(
    const float x1, const float y1, const float z1,
    const float x2, const float y2, const float z2,
    const float x3, const float y3, const float z3)
    :
    data
    {{
        {x1, y1, z1},
        {x2, y2, z2},
        {x3, y3, z3}
    }}
{}

inline constexpr Mat3::Mat3(const Quat& quat) :
    data
{{
    {1 - 2 * (quat.y * quat.y + quat.z * quat.z),     2 * (quat.x * quat.y + quat.z * quat.w),     2 * (quat.x * quat.z - quat.y * quat.w)},
    {    2 * (quat.x * quat.y - quat.z * quat.w), 1 - 2 * (quat.x * quat.x + quat.z * quat.z),     2 * (quat.y * quat.z + quat.x * quat.w)},
    {    2 * (quat.x * quat.z + quat.y * quat.w),     2 * (quat.y * quat.z - quat.x * quat.w), 1 - 2 * (quat.x * quat.x + quat.y * quat.y)},
}}
{}

inline constexpr Mat3::Mat3(const float value) :
    data
{{
    {value,   0.f,   0.f},
    {  0.f, value,   0.f},
    {  0.f,   0.f, value}
}}
{}

inline Vec3& Mat3::operator[](const size_t index)
{
    if (index > 3)
    {
        throw std::runtime_error("invalid index specified");
    }

    return data[index];
}

inline const Vec3& Mat3::operator[](const size_t index) const
{
    if (index > 3)
    {
        throw std::runtime_error("invalid index specified");
    }

    return data[index];
}

inline constexpr Mat3 operator*(const Mat3& lhs, const Mat3& rhs)
{
    return
    {
        rhs.data[0].x * lhs.data[0].x + rhs.data[0].y * lhs.data[1].x + rhs.data[0].z * lhs.data[2].x,
        rhs.data[0].x * lhs.data[0].y + rhs.data[0].y * lhs.data[1].y + rhs.data[0].z * lhs.data[2].y,
        rhs.data[0].x * lhs.data[0].z + rhs.data[0].y * lhs.data[1].z + rhs.data[0].z * lhs.data[2].z,

        rhs.data[1].x * lhs.data[0].x + rhs.data[1].y * lhs.data[1].x + rhs.data[1].z * lhs.data[2].x,
        rhs.data[1].x * lhs.data[0].y + rhs.data[1].y * lhs.data[1].y + rhs.data[1].z * lhs.data[2].y,
        rhs.data[1].x * lhs.data[0].z + rhs.data[1].y * lhs.data[1].z + rhs.data[1].z * lhs.data[2].z,

        rhs.data[2].x * lhs.data[0].x + rhs.data[2].y * lhs.data[1].x + rhs.data[2].z * lhs.data[2].x,
        rhs.data[2].x * lhs.data[0].y + rhs.data[2].y * lhs.data[1].y + rhs.data[2].z * lhs.data[2].y,
        rhs.data[2].x * lhs.data[0].z + rhs.data[2].y * lhs.data[1].z + rhs.data[2].z * lhs.data[2].z,
    };
}

inline constexpr Mat3 transpose(const Mat3& mat)
{
    return
    {
        mat.data[0].x, mat.data[1].x, mat.data[2].x,
        mat.data[0].y, mat.data[1].y, mat.data[2].y,
        mat.data[0].z, mat.data[1].z, mat.data[2].z,
    };
}

inline constexpr Mat3 translate(const Mat3& matrix, Vec2 translation)
{
    return
    {
        matrix[0].x                , matrix[0].y                , matrix[0].z,
        matrix[1].x                , matrix[1].y                , matrix[1].z,
        matrix[2].x + translation.x, matrix[2].y + translation.y, matrix[2].z,
    };
}

inline Mat3 inverse(const Mat3& mat)
{
    const auto det =
        + mat[0].x * (mat[1].y * mat[2].z - mat[1].z * mat[2].y)
        - mat[0].y * (mat[1].x * mat[2].z - mat[1].z * mat[2].x)
        + mat[0].z * (mat[1].x * mat[2].y - mat[1].y * mat[2].x);

    if (det == 0)
    {
        throw std::runtime_error{"singular matrix, can't find its inverse"};
    }
    return
    {
        (+(mat[1].y * mat[2].z - mat[2].y * mat[1].z)) / det, (-(mat[0].y * mat[2].z - mat[2].y * mat[0].z)) / det, (+(mat[0].y * mat[1].z - mat[1].y * mat[0].z)) / det,
        (-(mat[1].x * mat[2].z - mat[2].x * mat[1].z)) / det, (+(mat[0].x * mat[2].z - mat[2].x * mat[0].z)) / det, (-(mat[0].x * mat[1].z - mat[1].x * mat[0].z)) / det,
        (+(mat[1].x * mat[2].y - mat[2].x * mat[1].y)) / det, (-(mat[0].x * mat[2].y - mat[2].x * mat[0].y)) / det, (+(mat[0].x * mat[1].y - mat[1].x * mat[0].y)) / det,
    };
}

template<typename T>
inline Mat3 rotate(const Mat3& matrix, const Vec3& axis, T angle)
{
    const T c = std::cos(angle);
    const T s = std::sin(angle);
    const T t = 1 - c;

    const Vec3 normalizedAxis = normalize(axis);

    const T x = normalizedAxis.x;
    const T y = normalizedAxis.y;
    const T z = normalizedAxis.z;

    const T tx  = t * x ;
    const T ty  = t * y ;
    const T tz  = t * z ;
    const T txy = tx * y;
    const T txz = tx * z;
    const T tyz = ty * z;
    const T sx  = s * x ;
    const T sy  = s * y ;
    const T sz  = s * z ;

    const Mat3 rotationMatrix
    {
        tx * x + c, txy - sz  , txz + sy  ,
        txy + sz  , ty * y + c, tyz - sx  ,
        txz - sy  , tyz + sx  , tz * z + c,
    };

    return matrix * rotationMatrix;
}

inline std::string to_string(const Mat3& mat)
{
    return std::format("\n"
        "{:>10.7f}, {:>10.7f}, {:>10.7f},\n"
        "{:>10.7f}, {:>10.7f}, {:>10.7f},\n"
        "{:>10.7f}, {:>10.7f}, {:>10.7f}\n",
        mat[0].x, mat[0].y, mat[0].z,
        mat[1].x, mat[1].y, mat[1].z,
        mat[2].x, mat[2].y, mat[2].z);
}

inline constexpr Mat4::Mat4()
{}

inline constexpr Mat4::Mat4(
    const float x1, const float y1, const float z1, const float w1,
    const float x2, const float y2, const float z2, const float w2,
    const float x3, const float y3, const float z3, const float w3,
    const float x4, const float y4, const float z4, const float w4)
    :
    data
    {{
        {x1, y1, z1, w1},
        {x2, y2, z2, w2},
        {x3, y3, z3, w3},
        {x4, y4, z4, w4},
    }}
{}

inline constexpr Mat4::Mat4(const Quat& quat) :
    data
{{
    {1 - 2 * (quat.y * quat.y + quat.z * quat.z),     2 * (quat.x * quat.y + quat.z * quat.w),     2 * (quat.x * quat.z - quat.y * quat.w), 0},
    {    2 * (quat.x * quat.y - quat.z * quat.w), 1 - 2 * (quat.x * quat.x + quat.z * quat.z),     2 * (quat.y * quat.z + quat.x * quat.w), 0},
    {    2 * (quat.x * quat.z + quat.y * quat.w),     2 * (quat.y * quat.z - quat.x * quat.w), 1 - 2 * (quat.x * quat.x + quat.y * quat.y), 0},
    {                                          0,                                           0,                                           0, 1},
}}
{}

inline constexpr Mat4::Mat4(const float v) :
    data
{{
    {  v, 0.f, 0.f, 0.f},
    {0.f,   v, 0.f, 0.f},
    {0.f, 0.f,   v, 0.f},
    {0.f, 0.f, 0.f,   v}
}}
{}

inline constexpr Vec4::Vec4(const float v) : x{v}, y{v}, z{v}, w{v} {}
inline constexpr Vec4::Vec4(const float x_, const float y_, const float z_, const float w_) : x{x_}, y{y_}, z{z_}, w{w_} {}

inline Vec4& Mat4::operator[](const size_t index)
{
    if (index > 4)
    {
        throw std::runtime_error("invalid index specified");
    }

    return data[index];
}

inline const Vec4& Mat4::operator[](const size_t index) const
{
    if (index > 4)
    {
        throw std::runtime_error("invalid index specified");
    }

    return data[index];
}

inline constexpr Mat4 operator*(const Mat4& lhs, const Mat4& rhs)
{
    return
    {
        rhs.data[0].x * lhs.data[0].x + rhs.data[0].y * lhs.data[1].x + rhs.data[0].z * lhs.data[2].x + rhs.data[0].w * lhs.data[3].x,
        rhs.data[0].x * lhs.data[0].y + rhs.data[0].y * lhs.data[1].y + rhs.data[0].z * lhs.data[2].y + rhs.data[0].w * lhs.data[3].y,
        rhs.data[0].x * lhs.data[0].z + rhs.data[0].y * lhs.data[1].z + rhs.data[0].z * lhs.data[2].z + rhs.data[0].w * lhs.data[3].z,
        rhs.data[0].x * lhs.data[0].w + rhs.data[0].y * lhs.data[1].w + rhs.data[0].z * lhs.data[2].w + rhs.data[0].w * lhs.data[3].w,

        rhs.data[1].x * lhs.data[0].x + rhs.data[1].y * lhs.data[1].x + rhs.data[1].z * lhs.data[2].x + rhs.data[1].w * lhs.data[3].x,
        rhs.data[1].x * lhs.data[0].y + rhs.data[1].y * lhs.data[1].y + rhs.data[1].z * lhs.data[2].y + rhs.data[1].w * lhs.data[3].y,
        rhs.data[1].x * lhs.data[0].z + rhs.data[1].y * lhs.data[1].z + rhs.data[1].z * lhs.data[2].z + rhs.data[1].w * lhs.data[3].z,
        rhs.data[1].x * lhs.data[0].w + rhs.data[1].y * lhs.data[1].w + rhs.data[1].z * lhs.data[2].w + rhs.data[1].w * lhs.data[3].w,

        rhs.data[2].x * lhs.data[0].x + rhs.data[2].y * lhs.data[1].x + rhs.data[2].z * lhs.data[2].x + rhs.data[2].w * lhs.data[3].x,
        rhs.data[2].x * lhs.data[0].y + rhs.data[2].y * lhs.data[1].y + rhs.data[2].z * lhs.data[2].y + rhs.data[2].w * lhs.data[3].y,
        rhs.data[2].x * lhs.data[0].z + rhs.data[2].y * lhs.data[1].z + rhs.data[2].z * lhs.data[2].z + rhs.data[2].w * lhs.data[3].z,
        rhs.data[2].x * lhs.data[0].w + rhs.data[2].y * lhs.data[1].w + rhs.data[2].z * lhs.data[2].w + rhs.data[2].w * lhs.data[3].w,

        rhs.data[3].x * lhs.data[0].x + rhs.data[3].y * lhs.data[1].x + rhs.data[3].z * lhs.data[2].x + rhs.data[3].w * lhs.data[3].x,
        rhs.data[3].x * lhs.data[0].y + rhs.data[3].y * lhs.data[1].y + rhs.data[3].z * lhs.data[2].y + rhs.data[3].w * lhs.data[3].y,
        rhs.data[3].x * lhs.data[0].z + rhs.data[3].y * lhs.data[1].z + rhs.data[3].z * lhs.data[2].z + rhs.data[3].w * lhs.data[3].z,
        rhs.data[3].x * lhs.data[0].w + rhs.data[3].y * lhs.data[1].w + rhs.data[3].z * lhs.data[2].w + rhs.data[3].w * lhs.data[3].w,
    };
}

inline constexpr Mat4 transpose(const Mat4& mat)
{
    return
    {
        mat.data[0].x, mat.data[1].x, mat.data[2].x, mat.data[3].x,
        mat.data[0].y, mat.data[1].y, mat.data[2].y, mat.data[3].y,
        mat.data[0].z, mat.data[1].z, mat.data[2].z, mat.data[3].z,
        mat.data[0].w, mat.data[1].w, mat.data[2].w, mat.data[3].w,
    };
}

inline constexpr Mat4 translate(const Mat4& matrix, const Vec3& translation)
{
    return
    {
                        matrix[0].x,                 matrix[0].y,                 matrix[0].z, matrix[0].w,
                        matrix[1].x,                 matrix[1].y,                 matrix[1].z, matrix[1].w,
                        matrix[2].x,                 matrix[2].y,                 matrix[2].z, matrix[2].w,
        matrix[3].x + translation.x, matrix[3].y + translation.y, matrix[3].z + translation.z, matrix[3].w,
    };
}

<<<<<<< HEAD
<<<<<<< HEAD
template<typename T>
inline Mat4 rotate(const Mat4& matrix, const Vec3& axis, T angle)
{
    const T c = std::cos(angle);
    const T s = std::sin(angle);
    const T t = 1 - c;

    const Vec3 normalizedAxis = normalize(axis);

    const T x = normalizedAxis.x;
    const T y = normalizedAxis.y;
    const T z = normalizedAxis.z;

    const T tx  = t * x ;
    const T ty  = t * y ;
    const T tz  = t * z ;
    const T txy = tx * y;
    const T txz = tx * z;
    const T tyz = ty * z;
    const T sx  = s * x ;
    const T sy  = s * y ;
    const T sz  = s * z ;

    const Mat4 rotationMatrix
    {
        tx * x + c, txy - sz  , txz + sy  , 0,
        txy + sz  , ty * y + c, tyz - sx  , 0,
        txz - sy  , tyz + sx  , tz * z + c, 0,
        0         , 0         , 0         , 1,
    };

    return matrix * rotationMatrix;
=======
inline Mat4 scale(const Mat4& matrix, const Vec3& scaleVec)
=======
inline constexpr Mat4 scale(const Mat4& matrix, const Vec3& scaleVec)
>>>>>>> 9a2e018 (progress)
{
    const Mat4 scaleMatrix
    {
        scaleVec.x, 0         , 0         , 0,
        0         , scaleVec.y, 0         , 0,
        0         , 0         , scaleVec.z, 0,
        0         , 0         , 0         , 1,
    };

    return matrix * scaleMatrix;
>>>>>>> 7bc0c31 (progress)
}

inline std::string to_string(const Mat4 mat)
{
    return std::format("\n"
        "{:>10.7f}, {:>10.7f}, {:>10.7f}, {:>10.7f},\n"
        "{:>10.7f}, {:>10.7f}, {:>10.7f}, {:>10.7f},\n"
        "{:>10.7f}, {:>10.7f}, {:>10.7f}, {:>10.7f},\n"
        "{:>10.7f}, {:>10.7f}, {:>10.7f}, {:>10.7f}\n",
        mat[0].x, mat[0].y, mat[0].z, mat[0].w,
        mat[1].x, mat[1].y, mat[1].z, mat[1].w,
        mat[2].x, mat[2].y, mat[2].z, mat[2].w,
        mat[3].x, mat[3].y, mat[3].z, mat[3].w);
}
} // namespace ts