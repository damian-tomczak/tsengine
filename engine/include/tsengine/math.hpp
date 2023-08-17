#pragma once

// TODO: impl rotate func
// TODO: implement +* operators overloading

namespace ts::math
{
struct Vec3;
struct Vec4;

struct Vec2 final
{
    float x, y;

    constexpr Vec2() = default;
    constexpr Vec2(const float v);
    constexpr Vec2(const Vec3& vec3);
    constexpr Vec2(const Vec4& vec4);
    constexpr Vec2(const float x_, const float y_);
    [[nodiscard]] constexpr Vec2 operator*(const float scalar) const;
};

struct alignas(16) Vec3 final
{
    float x, y, z;

    constexpr Vec3() = default;
    constexpr Vec3(const float v);
    constexpr Vec3(const Vec4& vec4);
    constexpr Vec3(const float x_, const float y_, const float z_);
    [[nodiscard]] constexpr Vec3 operator*(const float scalar) const;
    [[nodiscard]] constexpr Vec3 operator+(const Vec3& vec3) const;
    constexpr Vec3& operator+=(const Vec3& rhs);
};

struct Vec4 final
{
    float x, y, z, w;

    constexpr Vec4() = default;
    constexpr Vec4(const float v);
    constexpr Vec4(const float x_, const float y_, const float z_, const float w_);
    [[nodiscard]] constexpr Vec4 operator*(const float scalar) const;
};

struct Quat
{
    float w, x, y, z;
};

template<size_t matRows, size_t matColumns>
struct Matrix
{
    static constexpr size_t rowsNum{matRows};
    static constexpr size_t colsNum{matColumns};
};

struct Mat2 : public Matrix<2, 2>
{
    std::array<Vec2, 2> data;

    Mat2()
    {}

    Mat2(
        const float x1, const float y1,
        const float x2, const float y2)
        :
        data
        {{
            {x1, y1},
            {x2, y2}
        }}
    {}

    explicit Mat2(const float value) :
        data
    {{
        {value, 0.f  },
        {0.f  , value}
    }}
    {}

    Vec2& operator[](const size_t index)
    {
        if ((index < 0) || (index > 2))
        {
            throw std::runtime_error("invalid index specified");
        }

        return data[index];
    }

    const Vec2& operator[](const size_t index) const
    {
        if ((index < 0) || (index > 2))
        {
            throw std::runtime_error("invalid index specified");
        }

        return data[index];
    }
};

inline Mat2 operator*(const Mat2& lhs, const Mat2& rhs)
{
    return
    {
        rhs.data[0].x * lhs.data[0].x + rhs.data[0].y * lhs.data[1].x,
        rhs.data[0].x * lhs.data[0].y + rhs.data[0].y * lhs.data[1].y,

        rhs.data[1].x * lhs.data[0].x + rhs.data[1].y * lhs.data[1].x,
        rhs.data[1].x * lhs.data[0].y + rhs.data[1].y * lhs.data[1].y,
    };
}

inline Vec2 normalize(const Vec2 vec)
{
    const auto mag = std::sqrt(vec.x * vec.x + vec.y * vec.y);
    return {vec.x / mag, vec.y / mag};
}

inline std::string to_string(const Mat2 mat)
{
    return std::format("\n"
        "{:>10.7f}, {:>10.7f},\n"
        "{:>10.7f}, {:>10.7f}\n",
        mat[0].x, mat[0].y,
        mat[1].x, mat[1].y);
}

struct Mat3 : public Matrix<3, 3>
{
    std::array<Vec3, 3> data{};

    Mat3()
    {}

    Mat3(
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

    Mat3(const Quat& quat) :
        data
        {{
            {1 - 2 * (quat.y * quat.y + quat.z * quat.z), 2 * (quat.x * quat.y + quat.z * quat.w)    , 2 * (quat.x * quat.z - quat.y * quat.w)    },
            {2 * (quat.x * quat.y - quat.z * quat.w)    , 1 - 2 * (quat.x * quat.x + quat.z * quat.z), 2 * (quat.y * quat.z + quat.x * quat.w)    },
            {2 * (quat.x * quat.z + quat.y * quat.w)    , 2 * (quat.y * quat.z - quat.x * quat.w)    , 1 - 2 * (quat.x * quat.x + quat.y * quat.y)},
        }}
    {}

    explicit Mat3(const float value) :
        data
    { {
        {value, 0.f  , 0.f  },
        {0.f  , value, 0.f  },
        {0.f  , 0.f  , value}
    } }
    {}

    Vec3& operator[](const size_t index)
    {
        if ((index < 0) || (index > 3))
        {
            throw std::runtime_error("invalid index specified");
        }

        return data[index];
    }

    const Vec3& operator[](const size_t index) const
    {
        if ((index < 0) || (index > 3))
        {
            throw std::runtime_error("invalid index specified");
        }

        return data[index];
    }
};

inline Mat3 operator*(const Mat3& lhs, const Mat3& rhs)
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

inline Mat3 translate(const Mat3& matrix, Vec2 translation)
{
    return
    {
        matrix[0].x                , matrix[0].y                , matrix[0].z,
        matrix[1].x                , matrix[1].y                , matrix[1].z,
        matrix[2].x + translation.x, matrix[2].y + translation.y, matrix[2].z,
    };
}

inline Vec3 normalize(const Vec3 vec)
{
    const auto mag = std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    return {vec.x / mag, vec.y / mag, vec.z / mag};
}

inline std::string to_string(const Mat3 mat)
{
    return std::format("\n"
        "{:>10.7f}, {:>10.7f}, {:>10.7f},\n"
        "{:>10.7f}, {:>10.7f}, {:>10.7f},\n"
        "{:>10.7f}, {:>10.7f}, {:>10.7f}\n",
        mat[0].x, mat[0].y, mat[0].z,
        mat[1].x, mat[1].y, mat[1].z,
        mat[2].x, mat[2].y, mat[2].z);
}

struct Mat4 : public Matrix<4, 4>
{
    std::array<Vec4, 4> data{};

    Mat4()
    {}

    Mat4(
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
            {x4, y4, z4, w4}
        }}
    {}

    Mat4(const Quat& quat) :
        data
        {{
            {1 - 2 * (quat.y * quat.y + quat.z * quat.z), 2 * (quat.x * quat.y + quat.z * quat.w)    , 2 * (quat.x * quat.z - quat.y * quat.w)    , 0},
            {2 * (quat.x * quat.y - quat.z * quat.w)    , 1 - 2 * (quat.x * quat.x + quat.z * quat.z), 2 * (quat.y * quat.z + quat.x * quat.w)    , 0},
            {2 * (quat.x * quat.z + quat.y * quat.w)    , 2 * (quat.y * quat.z - quat.x * quat.w)    , 1 - 2 * (quat.x * quat.x + quat.y * quat.y), 0},
            {0                                          , 0                                          , 0                                          , 1},
        }}
    {}

    Mat4(const float v) :
        data
        {{
            {v  , 0.f, 0.f, 0.f},
            {0.f, v  , 0.f, 0.f},
            {0.f, 0.f, v  , 0.f},
            {0.f, 0.f, 0.f, v  }
        }}
    {}

    Vec4& operator[](const size_t index)
    {
        if ((index < 0) || (index > 4))
        {
            throw std::runtime_error("invalid index specified");
        }

        return data[index];
    }

    const Vec4& operator[](const size_t index) const
    {
        if ((index < 0) || (index > 4))
        {
            throw std::runtime_error("invalid index specified");
        }

        return data[index];
    }
};

inline Mat4 operator*(const Mat4& lhs, const Mat4& rhs)
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

inline Mat4 translate(const Mat4& matrix, Vec3 translation)
{
    return
    {
        matrix[0].x                , matrix[0].y                , matrix[0].z                , matrix[0].w,
        matrix[1].x                , matrix[1].y                , matrix[1].z                , matrix[1].w,
        matrix[2].x                , matrix[2].y                , matrix[2].z                , matrix[2].w,
        matrix[3].x + translation.x, matrix[3].y + translation.y, matrix[3].z + translation.z, matrix[3].w,
    };
}

inline Vec4 normalize(const Vec4 vec)
{
    const auto mag = std::sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z + vec.w * vec.w);
    return {vec.x / mag, vec.y / mag, vec.z / mag, vec.w / mag};
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

template<typename T>
inline auto radians(T degrees)
{
    constexpr auto factor = std::numbers::pi / 180;
    return degrees * factor;
}

inline Mat4 scale(const Mat4& matrix, const Vec3& scaleVec)
{
    Mat4 scaleMatrix
    {
        scaleVec.x, 0         , 0         , 0  ,
        0         , scaleVec.y, 0         , 0  ,
        0         , 0         , scaleVec.z, 0  ,
        0         , 0         , 0         , 1.f
    };

    return matrix * scaleMatrix;
}

inline Mat2 inverse(const Mat2& mat)
{
    const auto det = mat[0].x * mat[1].y - mat[0].y * mat[1].x;

    if (det == 0)
    {
        throw std::runtime_error{"singular matrix, can't find its inverse"};
    }
    return
    {
       (+mat[1].y) / det, (-mat[0].y) / det,
       (-mat[1].x) / det, (+mat[0].x) / det,
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
        throw std::runtime_error{"Singular matrix, can't find its inverse"};
    }

    return
    {
        tempInverse00 / det, (-(mat[0].y * det23zw - mat[0].z * det23yw + mat[0].w * det23yz)) / det, (+(mat[0].y * det13zw - mat[0].z * det13yw + mat[0].w * det13yz)) / det, (-(mat[0].y * det12zw - mat[0].z * det12yw + mat[0].w * det12yz)) / det,
        tempInverse01 / det, (+(mat[0].x * det23zw - mat[0].z * det23xw + mat[0].w * det23xz)) / det, (-(mat[0].x * det13zw - mat[0].z * det13xw + mat[0].w * det13xz)) / det, (+(mat[0].x * det12zw - mat[0].z * det12xw + mat[0].w * det12xz)) / det,
        tempInverse02 / det, (-(mat[0].x * det23yw - mat[0].y * det23xw + mat[0].w * det23xy)) / det, (+(mat[0].x * det13yw - mat[0].y * det13xw + mat[0].w * det13xy)) / det, (-(mat[0].x * det12yw - mat[0].y * det12xw + mat[0].w * det12xy)) / det,
        tempInverse03 / det, (+(mat[0].x * det23yz - mat[0].y * det23xz + mat[0].z * det23xy)) / det, (-(mat[0].x * det13yz - mat[0].y * det13xz + mat[0].z * det13xy)) / det, (+(mat[0].x * det12yz - mat[0].y * det12xz + mat[0].z * det12xy)) / det,
    };
}

inline constexpr Vec2::Vec2(const float v) : x{v}, y{v}
{}

inline constexpr Vec2::Vec2(const Vec3& vec3) : x{vec3.x}, y{vec3.y}
{}

inline constexpr Vec2::Vec2(const Vec4& vec4) : x{vec4.x}, y{vec4.y}
{}

inline constexpr Vec2::Vec2(const float x_, const float y_) : x{x_}, y{y_}
{}

inline constexpr Vec2 Vec2::operator*(const float scalar) const
{
    return {x * scalar, y * scalar};
}

inline constexpr Vec3::Vec3(const float v) : x{v}, y{v}, z{v}
{}

inline constexpr Vec3::Vec3(const Vec4& vec4) : x{ vec4.x }, y{ vec4.y }, z{ vec4.z }
{}

inline constexpr Vec3::Vec3(const float x_, const float y_, const float z_) : x{ x_ }, y{ y_ }, z{ z_ }
{}

inline constexpr Vec3 Vec3::operator*(const float scalar) const
{
    return {x * scalar, y * scalar, z * scalar};
}

inline constexpr Vec3 Vec3::operator+(const Vec3& rhs) const
{
    return { x + rhs.x, y * rhs.y, z + rhs.z };
}

inline constexpr Vec3& Vec3::operator+=(const Vec3& rhs)
{
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
}

inline constexpr Vec4::Vec4(const float v) : x{v}, y{v}, z{v}, w{v}
{}

inline constexpr Vec4::Vec4(const float x_, const float y_, const float z_, const float w_) :
    x{x_}, y{y_}, z{z_}, w{w_}
{}

inline constexpr Vec4 Vec4::operator*(const float scalar) const
{
    return {x * scalar, y * scalar, z * scalar, w * scalar};
}
} // namespace ts