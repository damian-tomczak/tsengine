#pragma once

/**
 * Mathematical solutions grow with the needs of the codebase.
 * Header is slightly inspired by glm, but isn't so optimized
 * functions, classes and structures marked as _ shouldn't be used outside
 */

namespace ts::math
{
template<typename T = float>
struct Vec2
{
    T x;
    T y;

    template<typename T>
    Vec2 operator*(T scalar) const { return {x * scalar, y * scalar}; }
};

template<typename T = float>
struct Vec3
{
    T x;
    T y;
    T z;
};

template<typename T>
struct Quat;

template<typename ValueT, size_t matRows, size_t matColumns>
struct Matrix
{
    using ValueType = ValueT;

    static constexpr size_t rowsNum{matRows};
    static constexpr size_t colsNum{matColumns};

    using Row = std::array<ValueT, colsNum>;
    using Mat = std::array<Row, rowsNum>;
};

template<typename ValueT = float>
struct Mat4 : public Matrix<ValueT, 4, 4>
{
    using ValueType = typename Matrix<ValueT, 4, 4>::ValueType;

    using Row = typename Matrix<ValueT, 4, 4>::Row;
    using Mat = typename Matrix<ValueT, 4, 4>::Mat;

    Mat data;

    Mat4()
    {}

    Mat4(const Mat& mat) : data(mat)
    {}

    Mat4(const Quat<ValueT>& quat) :
        data{
            1 - 2 * (quat.y * quat.y + quat.z * quat.z), 2 * (quat.x * quat.y + quat.z * quat.w)    , 2 * (quat.x * quat.z - quat.y * quat.w)    , 0,
            2 * (quat.x * quat.y - quat.z * quat.w)    , 1 - 2 * (quat.x * quat.x + quat.z * quat.z), 2 * (quat.y * quat.z + quat.x * quat.w)    , 0,
            2 * (quat.x * quat.z + quat.y * quat.w)    , 2 * (quat.y * quat.z - quat.x * quat.w)    , 1 - 2 * (quat.x * quat.x + quat.y * quat.y), 0,
            0                                          , 0                                          , 0                                          , 1,
        }
    {}

    static Mat4<ValueT> makeScalarMat(const ValueT value)
    {
        return {
        {{
            value, 0    , 0    , 0    ,
            0    , value, 0    , 0    ,
            0    , 0    , value, 0    ,
            0    , 0    , 0    , value,
        }}};
    }

    Row& operator[](const int32_t index)
    {
        return data[index];
    }

    const std::array<ValueT, 4>& operator[](const int32_t index) const
    {
        return data[index];
    }

    Mat4 operator*(const Mat4& rhs) const {
        return {{
            data[0][0] * rhs.data[0][0] + data[0][1] * rhs.data[1][0] + data[0][2] * rhs.data[2][0] + data[0][3] * rhs.data[3][0],
            data[0][0] * rhs.data[0][1] + data[0][1] * rhs.data[1][1] + data[0][2] * rhs.data[2][1] + data[0][3] * rhs.data[3][1],
            data[0][0] * rhs.data[0][2] + data[0][1] * rhs.data[1][2] + data[0][2] * rhs.data[2][2] + data[0][3] * rhs.data[3][2],
            data[0][0] * rhs.data[0][3] + data[0][1] * rhs.data[1][3] + data[0][2] * rhs.data[2][3] + data[0][3] * rhs.data[3][3],

            data[1][0] * rhs.data[0][0] + data[1][1] * rhs.data[1][0] + data[1][2] * rhs.data[2][0] + data[1][3] * rhs.data[3][0],
            data[1][0] * rhs.data[0][1] + data[1][1] * rhs.data[1][1] + data[1][2] * rhs.data[2][1] + data[1][3] * rhs.data[3][1],
            data[1][0] * rhs.data[0][2] + data[1][1] * rhs.data[1][2] + data[1][2] * rhs.data[2][2] + data[1][3] * rhs.data[3][2],
            data[1][0] * rhs.data[0][3] + data[1][1] * rhs.data[1][3] + data[1][2] * rhs.data[2][3] + data[1][3] * rhs.data[3][3],

            data[2][0] * rhs.data[0][0] + data[2][1] * rhs.data[1][0] + data[2][2] * rhs.data[2][0] + data[2][3] * rhs.data[3][0],
            data[2][0] * rhs.data[0][1] + data[2][1] * rhs.data[1][1] + data[2][2] * rhs.data[2][1] + data[2][3] * rhs.data[3][1],
            data[2][0] * rhs.data[0][2] + data[2][1] * rhs.data[1][2] + data[2][2] * rhs.data[2][2] + data[2][3] * rhs.data[3][2],
            data[2][0] * rhs.data[0][3] + data[2][1] * rhs.data[1][3] + data[2][2] * rhs.data[2][3] + data[2][3] * rhs.data[3][3],

            data[3][0] * rhs.data[0][0] + data[3][1] * rhs.data[1][0] + data[3][2] * rhs.data[2][0] + data[3][3] * rhs.data[3][0],
            data[3][0] * rhs.data[0][1] + data[3][1] * rhs.data[1][1] + data[3][2] * rhs.data[2][1] + data[3][3] * rhs.data[3][1],
            data[3][0] * rhs.data[0][2] + data[3][1] * rhs.data[1][2] + data[3][2] * rhs.data[2][2] + data[3][3] * rhs.data[3][2],
            data[3][0] * rhs.data[0][3] + data[3][1] * rhs.data[1][3] + data[3][2] * rhs.data[2][3] + data[3][3] * rhs.data[3][3],
        }};
    }
};

template<typename T = float, typename Z = float>
inline Mat4<T> translate(const Mat4<T>& matrix, Vec3<Z> translation)
{
    return {
    {{
        matrix[0][0]                , matrix[0][1]                , matrix[0][2]                , matrix[0][3],
        matrix[1][0]                , matrix[1][1]                , matrix[1][2]                , matrix[1][3],
        matrix[2][0]                , matrix[2][1]                , matrix[2][2]                , matrix[2][3],
        matrix[3][0] + translation.x, matrix[3][1] + translation.y, matrix[3][2] + translation.z, matrix[3][3],
    }}};
}

template<typename ParameterT, typename ReturnT = float>
inline ReturnT radians(ParameterT degrees)
{
    constexpr auto factor{std::numbers::pi / 180};
    return degrees * factor;
}

template<typename T = float>
struct Quat
{
    T w, x, y, z;
};

template<typename Mat>
inline void _getCofactor(
    const Mat& referenceMat,
    Mat& newMat,
    size_t referenceRow,
    size_t referenceColumn,
    size_t depth)
{
    //for (size_t row{}, newMatRow{}; row < depth; ++row)
    //{
    //    for (size_t col{}, newMatCol{}; col < depth; ++col)
    //    {
    //        if ((row != referenceRow) && (col != referenceColumn))
    //        {
    //            newMat[newMatCol][newMatRow++] = referenceMat[row][col];

    //            if (newMatRow == depth - 1)
    //            {
    //                newMatRow = 0;
    //                newMatCol++;
    //            }
    //        }
    //    }
    //}
}

template<typename Mat>
inline auto _determinant(const Mat& mat, size_t depth)
{
    typename Mat::ValueType det{};

    if (depth == 1)
    {
        return mat[0][0];
    }

    bool sign{};
    Mat temp;
    for (size_t f{}; f < depth; ++f)
    {
        _getCofactor(mat, temp, 0ll, f, depth);
        det += sign * mat[0][f] * _determinant(temp, depth - 1);

        sign = !sign;
    }

    return det;
}


template<typename Mat>
inline void _adjoint(const Mat& referenceMat, Mat& adjMat)
{
    //static_assert(std::is_base_of<Mat<typename Mat::value_type, Mat::rowsNum, Mat::colsNum>, Matrix>::value, "Mat should be a type derived from Mat");

    //Mat temp;
    //for (size_t row{}; row < Mat::rowsNum ; ++row)
    //{
    //    for (size_t col{}; col < Mat::colsNum; ++col)
    //    {
    //        _getCofactor(referenceMat, temp, row, col, Mat::rowsNum);

    //        const auto sign{((row + col) % 2 == 0) ? 1 : -1};

    //        adjMat[col][row] = sign * (_determinant(temp, Mat::rowsNum - 1));
    //    }
    //}
}

inline Mat4<> inverse(const Mat4<>& mat)
{
    const auto det{_determinant(mat, 4)};
    if (det == 0)
    {
        throw std::runtime_error{"Singular matrix, can't find its inverse"};
    }

    Mat4<> adjMat;
    _adjoint(mat, adjMat);

    return {{{
        adjMat[0][0] / det, adjMat[0][1] / det, adjMat[0][2] / det, adjMat[0][3] / det,
        adjMat[1][0] / det, adjMat[1][1] / det, adjMat[1][2] / det, adjMat[1][3] / det,
        adjMat[2][0] / det, adjMat[2][1] / det, adjMat[2][2] / det, adjMat[2][3] / det,
        adjMat[3][0] / det, adjMat[3][1] / det, adjMat[3][2] / det, adjMat[3][3] / det,
    }}};
}
} // namespace ts