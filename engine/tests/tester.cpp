#include "gtest/gtest.h"
#include "tsengine/math.hpp"

TEST(DummyTests, Dummytest)
{
    const auto multiplication = 28 * 10 * 2000;
    const auto expected = 560000;
    ASSERT_EQ(expected, multiplication);
}

TEST(MathTests, mat4MultiplicationTest)
{
    ts::math::Mat4 leftMatrix
    {
        1, 2, 3, 4,
        1, 2, 3, 4,
        5, 6, 7, 8,
        5, 6, 7, 8,
    };

    ts::math::Mat4 rightMatrix
    {
        1, 2, 3, 4,
        1, 2, 3, 4,
        5, 6, 7, 8,
        5, 6, 7, 8,
    };

    ts::math::Mat4 expected
    {
        38, 48 , 58 , 68 ,
        38, 48 , 58 , 68 ,
        86, 112, 138, 164,
        86, 112, 138, 164,
    };
    const auto multiplication = rightMatrix * leftMatrix;
    ASSERT_EQ(ts::math::to_string(expected), ts::math::to_string(multiplication));
}

TEST(MathTests, mat3MultiplicationTest)
{
    ts::math::Mat3 leftMatrix
    {
        0.4f, 0.5f, 0.6f,
        0.7f, 0.8f, 0.9f,
        0.1f, 0.2f, 0.3f,
    };

    ts::math::Mat3 rightMatrix
    {
        0.11f, 0.12f, 0.13f,
        0.14f, 0.15f, 0.16f,
        0.17f, 0.18f, 0.19f,
    };

    ts::math::Mat3 expected
    {
        0.216f, 0.231f, 0.246f,
        0.342f, 0.366f, 0.39f ,
        0.09f , 0.096f, 0.102f,
    };
    const auto multiplication = rightMatrix * leftMatrix;
    ASSERT_EQ(ts::math::to_string(expected), ts::math::to_string(multiplication));
}

TEST(MathTests, mat2InversionTest)
{

    ts::math::Mat2 matrix
    {
        1.0f, 3.0f,
        4.0f, 2.0f,
    };

    ts::math::Mat2 identityMatrix
    {
        1.0f, 0.0f,
        0.0f, 1.0f,
    };
    const auto invertedMatrix = ts::math::inverse(matrix);
    const auto s = matrix * invertedMatrix;
    ASSERT_EQ(ts::math::to_string(identityMatrix), ts::math::to_string(s));
}

TEST(MathTests, mat3InversionTest)
{

    ts::math::Mat3 matrix
    {
        1.0f, 2.0f, 3.0f,
        0.0f, 1.0f, 4.0f,
        5.0f, 6.0f, 0.0f,
    };

    ts::math::Mat3 expected
    {
        -24.0f, +18.0f, +5.0f,
        +20.0f, -15.0f, -4.0f,
        -5.0f , +4.0f , +1.0f,
    };
    const auto invertedMatrix = ts::math::inverse(matrix);
    ASSERT_EQ(ts::math::to_string(invertedMatrix), ts::math::to_string(expected));
}

TEST(MathTests, mat4InversionTest)
{

    ts::math::Mat4 matrix
    {
        3.0f, 4.0f, 2.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        2.0f, 1.0f, 3.0f, 2.0f,
        1.0f, 0.0f, 1.0f, 1.0f,
    };

    ts::math::Mat4 expected
    {
        +0.5f, -1.5f, -0.5f, +0.5f,
        -0.0f, +1.0f, -0.0f, +0.0f,
        +0.0f, -1.0f, +1.0f, -2.0f,
        -0.5f, +2.5f, -0.5f, +2.5f,
    };
    const auto invertedMatrix = ts::math::inverse(matrix);
    ASSERT_EQ(ts::math::to_string(invertedMatrix), ts::math::to_string(expected));
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}