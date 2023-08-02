#pragma once

/**
 * Mathematical solutions grow with the needs of the codebase.
 * Header is slightly inspired by glm, but isn't so optimized
 * functions, classes and structures marked as _ shouldn't be used outside
 */

namespace ts::math
{
struct Vec2
{
    float x{}, y{};

    template<typename T>
    Vec2 operator*(T scalar) const { return {x * scalar, y * scalar}; }
};

struct Vec4
{
    float x{}, y{}, z{}, w{};

    template<typename T>
    Vec4(const std::array<T, 4>& rhs) : x{ rhs[0] }, y{ rhs[1] }, z{ rhs[2] }, w{ rhs[3] }
    {}

    Vec4(std::initializer_list<float> list)
    {
        if (list.size() == 4)
        {
            auto it = list.begin();
            x = *it++;
            y = *it++;
            z = *it++;
            w = *it++;
        }
        else
        {
            throw std::invalid_argument("initializer list must contain exactly 4 elements");
        }
    }
};

struct Vec3
{
    float x{}, y{}, z{};

    Vec3() {};

    Vec3(const Vec4& vec4) : x{vec4.x}, y{vec4.y}, z{vec4.z}
    {}

    Vec3(std::initializer_list<float> list)
    {
        if (list.size() == 3)
        {
            auto it = list.begin();
            x = *it++;
            y = *it++;
            z = *it++;
        }
        else
        {
            throw std::invalid_argument("initializer list must contain exactly 4 elements");
        }
    }

    template<typename T>
    Vec3 operator*(T scalar) const { return { x * scalar, y * scalar, z * scalar }; }
};

struct Quat
{
    float w, x, y, z;
};

struct BaseMatrix {};

template<size_t matRows, size_t matColumns>
struct Matrix : BaseMatrix
{
    static constexpr size_t rowsNum{matRows};
    static constexpr size_t colsNum{matColumns};

    using Row = std::array<float, colsNum>;
    using Mat = std::array<Row, rowsNum>;
};

struct Mat4 : public Matrix<4, 4>
{
    using Row = typename Matrix<4, 4>::Row;
    using Mat = typename Matrix<4, 4>::Mat;

    Mat data{};

    Mat4()
    {}

    Mat4(const Mat& mat) : data(mat)
    {}

    explicit Mat4(const Quat& quat) :
        data{
            1 - 2 * (quat.y * quat.y + quat.z * quat.z), 2 * (quat.x * quat.y + quat.z * quat.w)    , 2 * (quat.x * quat.z - quat.y * quat.w)    , 0,
            2 * (quat.x * quat.y - quat.z * quat.w)    , 1 - 2 * (quat.x * quat.x + quat.z * quat.z), 2 * (quat.y * quat.z + quat.x * quat.w)    , 0,
            2 * (quat.x * quat.z + quat.y * quat.w)    , 2 * (quat.y * quat.z - quat.x * quat.w)    , 1 - 2 * (quat.x * quat.x + quat.y * quat.y), 0,
            0                                          , 0                                          , 0                                          , 1,
        }
    {}

    explicit Mat4(const float value) :
        data{
            value, 0.f  , 0.f  , 0.f  ,
            0.f  , value, 0.f  , 0.f  ,
            0.f  , 0.f  , value, 0.f  ,
            0.f  , 0.f  , 0.f  , value
        }
    {}

    Row& operator[](const size_t index)
    {
        return data[index];
    }

    const Row& operator[](const size_t index) const
    {
        return data[index];
    }
};

inline Mat4 operator*(const Mat4& lhs, const Mat4& rhs)
{
    return {{
        rhs.data[0][0] * lhs.data[0][0] + rhs.data[0][1] * lhs.data[1][0] + rhs.data[0][2] * lhs.data[2][0] + rhs.data[0][3] * lhs.data[3][0],
        rhs.data[0][0] * lhs.data[0][1] + rhs.data[0][1] * lhs.data[1][1] + rhs.data[0][2] * lhs.data[2][1] + rhs.data[0][3] * lhs.data[3][1],
        rhs.data[0][0] * lhs.data[0][2] + rhs.data[0][1] * lhs.data[1][2] + rhs.data[0][2] * lhs.data[2][2] + rhs.data[0][3] * lhs.data[3][2],
        rhs.data[0][0] * lhs.data[0][3] + rhs.data[0][1] * lhs.data[1][3] + rhs.data[0][2] * lhs.data[2][3] + rhs.data[0][3] * lhs.data[3][3],

        rhs.data[1][0] * lhs.data[0][0] + rhs.data[1][1] * lhs.data[1][0] + rhs.data[1][2] * lhs.data[2][0] + rhs.data[1][3] * lhs.data[3][0],
        rhs.data[1][0] * lhs.data[0][1] + rhs.data[1][1] * lhs.data[1][1] + rhs.data[1][2] * lhs.data[2][1] + rhs.data[1][3] * lhs.data[3][1],
        rhs.data[1][0] * lhs.data[0][2] + rhs.data[1][1] * lhs.data[1][2] + rhs.data[1][2] * lhs.data[2][2] + rhs.data[1][3] * lhs.data[3][2],
        rhs.data[1][0] * lhs.data[0][3] + rhs.data[1][1] * lhs.data[1][3] + rhs.data[1][2] * lhs.data[2][3] + rhs.data[1][3] * lhs.data[3][3],

        rhs.data[2][0] * lhs.data[0][0] + rhs.data[2][1] * lhs.data[1][0] + rhs.data[2][2] * lhs.data[2][0] + rhs.data[2][3] * lhs.data[3][0],
        rhs.data[2][0] * lhs.data[0][1] + rhs.data[2][1] * lhs.data[1][1] + rhs.data[2][2] * lhs.data[2][1] + rhs.data[2][3] * lhs.data[3][1],
        rhs.data[2][0] * lhs.data[0][2] + rhs.data[2][1] * lhs.data[1][2] + rhs.data[2][2] * lhs.data[2][2] + rhs.data[2][3] * lhs.data[3][2],
        rhs.data[2][0] * lhs.data[0][3] + rhs.data[2][1] * lhs.data[1][3] + rhs.data[2][2] * lhs.data[2][3] + rhs.data[2][3] * lhs.data[3][3],

        rhs.data[3][0] * lhs.data[0][0] + rhs.data[3][1] * lhs.data[1][0] + rhs.data[3][2] * lhs.data[2][0] + rhs.data[3][3] * lhs.data[3][0],
        rhs.data[3][0] * lhs.data[0][1] + rhs.data[3][1] * lhs.data[1][1] + rhs.data[3][2] * lhs.data[2][1] + rhs.data[3][3] * lhs.data[3][1],
        rhs.data[3][0] * lhs.data[0][2] + rhs.data[3][1] * lhs.data[1][2] + rhs.data[3][2] * lhs.data[2][2] + rhs.data[3][3] * lhs.data[3][2],
        rhs.data[3][0] * lhs.data[0][3] + rhs.data[3][1] * lhs.data[1][3] + rhs.data[3][2] * lhs.data[2][3] + rhs.data[3][3] * lhs.data[3][3],
    }};
}

inline Mat4 translate(const Mat4& matrix, Vec3 translation)
{
    return {
    {{
        matrix[0][0]                , matrix[0][1]                , matrix[0][2]                , matrix[0][3],
        matrix[1][0]                , matrix[1][1]                , matrix[1][2]                , matrix[1][3],
        matrix[2][0]                , matrix[2][1]                , matrix[2][2]                , matrix[2][3],
        matrix[3][0] + translation.x, matrix[3][1] + translation.y, matrix[3][2] + translation.z, matrix[3][3],
    }}};
}

template<typename T>
inline auto radians(T degrees)
{
    constexpr auto factor{std::numbers::pi / 180};
    return degrees * factor;
}

inline Mat4 scale(const Mat4& matrix, const Vec3& scaleVec)
{
    Mat4 scaleMatrix{{{
        scaleVec.x, 0            , 0         , 0,
        0         , scaleVec.y   , 0         , 0,
        0         , 0            , scaleVec.z, 0,
        0         , 0            , 0         , 1.f
    }}};

    return matrix * scaleMatrix;
}

template<typename Mat>
inline float _determinant(const Mat& mat, size_t depth);

template<typename Mat>
inline void _adjoint(const Mat& referenceMat, Mat& adjMat);

inline Mat4 inverse(const Mat4& mat)
{
    const auto det = _determinant(mat, 4);
    if (det == 0)
    {
        throw std::runtime_error{"singular matrix, can't find its inverse"};
    }

    Mat4 adjMat;
    _adjoint(mat, adjMat);

    return {{{
        adjMat[0][0] / det, adjMat[0][1] / det, adjMat[0][2] / det, adjMat[0][3] / det,
        adjMat[1][0] / det, adjMat[1][1] / det, adjMat[1][2] / det, adjMat[1][3] / det,
        adjMat[2][0] / det, adjMat[2][1] / det, adjMat[2][2] / det, adjMat[2][3] / det,
        adjMat[3][0] / det, adjMat[3][1] / det, adjMat[3][2] / det, adjMat[3][3] / det,
    }}};
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
        mat[0][0], mat[0][1], mat[0][2], mat[0][3],
        mat[1][0], mat[1][1], mat[1][2], mat[1][3],
        mat[2][0], mat[2][1], mat[2][2], mat[2][3],
        mat[3][0], mat[3][1], mat[3][2], mat[3][3]);
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
                newMat[newMatRow][newMatCol++] = referenceMat[row][col];

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
        return mat[0][0];
    }

    int32_t sign = 1;
    Mat temp;
    for (size_t f{}; f < depth; ++f)
    {
        _getCofactor(mat, temp, 0, f, depth);
        det += sign * mat[0][f] * _determinant(temp, depth - 1);

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

            adjMat[col][row] = sign * (_determinant(temp, Mat::rowsNum - 1));
        }
    }
}
} // namespace ts