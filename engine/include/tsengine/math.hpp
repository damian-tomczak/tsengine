#pragma once

namespace ts::math
{
struct Vec3;
struct Vec4;

struct Vec2 final
{
    float x{}, y{};

    Vec2();
    Vec2(const float x_, const float y_);
    Vec2 operator*(const float scalar) const;
};

struct Vec3 final
{
    float x{}, y{}, z{};

    Vec3();
    Vec3(const Vec4& vec4);
    Vec3(const float x_, const float y_, const float z_);
    Vec3 operator*(const float scalar) const;
};

struct Vec4 final
{
    float x{}, y{}, z{}, w{};

    Vec4();
    Vec4(const float x_, const float y_, const float z_, const float w_);
};

struct Quat
{
    float w{}, x{}, y{}, z{};
};

inline Vec2::Vec2()
{}

inline Vec2::Vec2(const float x_, const float y_) : x{x_}, y{y_}
{}

inline Vec2 Vec2::operator*(const float scalar) const
{
    return {x * scalar, y * scalar};
}

inline Vec3::Vec3()
{}

inline Vec3::Vec3(const Vec4& vec4) : x{vec4.x}, y{vec4.y}, z{vec4.z}
{}

inline Vec3::Vec3(const float x_, const float y_, const float z_) : x{x_}, y{y_}, z{z_}
{}

inline Vec3 Vec3::operator*(const float scalar) const
{
    return {x * scalar, y * scalar, z * scalar};
}

inline Vec4::Vec4()
{}

inline Vec4::Vec4(const float x_, const float y_, const float z_, const float w_) :
    x{x_}, y{y_}, z{z_}, w{w_}
{}

struct BaseMatrix {};

template<size_t matRows, size_t matColumns>
struct Matrix : BaseMatrix
{
    static constexpr size_t rowsNum{matRows};
    static constexpr size_t colsNum{matColumns};
};

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

    explicit Mat4(const float value) :
        data
        {{
            {value, 0.f  , 0.f  , 0.f  },
            {0.f  , value, 0.f  , 0.f  },
            {0.f  , 0.f  , value, 0.f  },
            {0.f  , 0.f  , 0.f  , value}
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
    return { vec.x / mag, vec.y / mag, vec.z / mag, vec.w / mag };
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
        scaleVec.x, 0            , 0         , 0,
        0         , scaleVec.y   , 0         , 0,
        0         , 0            , scaleVec.z, 0,
        0         , 0            , 0         , 1.f
    };

    return matrix * scaleMatrix;
}


template<typename Mat>
inline float _determinant(const Mat& mat, size_t depth);

template<typename Mat>
inline void _adjoint(const Mat& referenceMat, Mat& adjMat);

// TODO: refactor
inline Mat4 inverse(const Mat4& mat)
{
    const auto det = _determinant(mat, 4);
    if (det == 0)
    {
        throw std::runtime_error{"singular matrix, can't find its inverse"};
    }

    Mat4 adjMat;
    _adjoint(mat, adjMat);

    return
    {
        adjMat[0].x / det, adjMat[0].y / det, adjMat[0].z / det, adjMat[0].w / det,
        adjMat[1].x / det, adjMat[1].y / det, adjMat[1].z / det, adjMat[1].w / det,
        adjMat[2].x / det, adjMat[2].y / det, adjMat[2].z / det, adjMat[2].w / det,
        adjMat[3].x / det, adjMat[3].y / det, adjMat[3].z / det, adjMat[3].w / det,
    };
}

template<typename Mat>
inline void _getCofactor(
    const Mat& referenceMat,
    Mat& newMat,
    size_t referenceRow,
    size_t referenceColumn,
    size_t depth)
{
    static_assert(std::is_base_of_v<BaseMatrix, Mat>, "invalid template parameter");

    for (size_t row{}, newMatRow{}; row < depth; ++row)
    {
        for (size_t col{}, newMatCol{}; col < depth; ++col)
        {
            if ((row != referenceRow) && (col != referenceColumn))
            {
                float referenceValue{};
                switch(col)
                {
                case 0: referenceValue = referenceMat[row].x; break;
                case 1: referenceValue = referenceMat[row].y; break;
                case 2: referenceValue = referenceMat[row].z; break;
                case 3: referenceValue = referenceMat[row].w; break;
                }

                switch (newMatCol)
                {
                case 0: newMat[newMatRow].x = referenceValue; break;
                case 1: newMat[newMatRow].y = referenceValue; break;
                case 2: newMat[newMatRow].z = referenceValue; break;
                case 3: newMat[newMatRow].w = referenceValue; break;
                default:
                    throw std::runtime_error{"invalid depth"};
                }
                newMatCol++;

                if (newMatCol == depth - 1)
                {
                    newMatCol = 0;
                    newMatRow++;
                }
            }
        }
    }
}

template<typename Mat>
inline float _determinant(const Mat& mat, size_t depth)
{
    static_assert(std::is_base_of_v<BaseMatrix, Mat>, "invalid template parameter");

    float det{};

    if (depth == 1)
    {
        return mat[0].x;
    }

    int32_t sign = 1;
    Mat temp;
    for (size_t f{}; f < depth; ++f)
    {
        _getCofactor(mat, temp, 0, f, depth);
        float major{};
        switch (f)
        {
        case 0: major = mat[0].x; break;
        case 1: major = mat[0].y; break;
        case 2: major = mat[0].z; break;
        case 3: major = mat[0].w; break;
        default:
            throw std::runtime_error{ "invalid depth" };
        }

        det += sign * major * _determinant(temp, depth - 1);

        sign = -sign;
    }

    return det;
}


template<typename Mat>
inline void _adjoint(const Mat& referenceMat, Mat& adjMat)
{
    static_assert(std::is_base_of_v<BaseMatrix, Mat>, "invalid template parameter");

    Mat temp;
    for (size_t row{}; row < Mat::rowsNum ; ++row)
    {
        for (size_t col{}; col < Mat::colsNum; ++col)
        {
            _getCofactor(referenceMat, temp, row, col, Mat::rowsNum);

            const auto sign = ((row + col) % 2 == 0) ? 1 : -1;

            switch(row)
            {
            case 0: adjMat[col].x = sign * (_determinant(temp, Mat::rowsNum - 1)); break;
            case 1: adjMat[col].y = sign * (_determinant(temp, Mat::rowsNum - 1)); break;
            case 2: adjMat[col].z = sign * (_determinant(temp, Mat::rowsNum - 1)); break;
            case 3: adjMat[col].w = sign * (_determinant(temp, Mat::rowsNum - 1)); break;
            default:
                throw std::runtime_error{"invalid component"};
            }
        }
    }
}
} // namespace ts