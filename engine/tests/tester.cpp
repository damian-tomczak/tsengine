#include "gtest/gtest.h"
#include "tests_core_adapter.h"
#include "tsengine/math.hpp"

#include <memory>

TEST(DummyTests, Dummytest)
{
    const auto multiplication = 28 * 10 * 2000;
    const auto expected = 560000;

    ASSERT_EQ(expected, multiplication);
}

TEST(MathTests, mat4MultiplicationTest)
{
    const ts::math::Mat4 leftMatrix
    {
        1, 2, 3, 4,
        1, 2, 3, 4,
        5, 6, 7, 8,
        5, 6, 7, 8,
    };

    const ts::math::Mat4 rightMatrix
    {
        1, 2, 3, 4,
        1, 2, 3, 4,
        5, 6, 7, 8,
        5, 6, 7, 8,
    };

    const ts::math::Mat4 expected
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
    const ts::math::Mat3 leftMatrix
    {
        0.4f, 0.5f, 0.6f,
        0.7f, 0.8f, 0.9f,
        0.1f, 0.2f, 0.3f,
    };

    const ts::math::Mat3 rightMatrix
    {
        0.11f, 0.12f, 0.13f,
        0.14f, 0.15f, 0.16f,
        0.17f, 0.18f, 0.19f,
    };

    const ts::math::Mat3 expected
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
    const ts::math::Mat2 matrix
    {
        1.f, 3.f,
        4.f, 2.f,
    };

    const ts::math::Mat2 identityMatrix
    {
        1.f, 0.f,
        0.f, 1.f,
    };

    const auto invertedMatrix = ts::math::inverse(matrix);
    const auto s = matrix * invertedMatrix;
    ASSERT_EQ(ts::math::to_string(identityMatrix), ts::math::to_string(s));
}

TEST(MathTests, mat3InversionTest)
{
    const ts::math::Mat3 matrix
    {
        1.f, 2.f, 3.f,
        0.f, 1.f, 4.f,
        5.f, 6.f, 0.f,
    };

    const ts::math::Mat3 expected
    {
        -24.f, +18.f, +5.f,
        +20.f, -15.f, -4.f,
        -5.f , +4.f , +1.f,
    };

    const auto invertedMatrix = ts::math::inverse(matrix);
    ASSERT_EQ(ts::math::to_string(invertedMatrix), ts::math::to_string(expected));
}

TEST(MathTests, mat4InversionTest)
{
    const ts::math::Mat4 matrix
    {
        3.f, 4.f, 2.f, 1.f,
        0.f, 1.f, 0.f, 0.f,
        2.f, 1.f, 3.f, 2.f,
        1.f, 0.f, 1.f, 1.f,
    };

    const ts::math::Mat4 expected
    {
        +0.5f, -1.5f, -0.5f, +0.5f,
        -0.f , +1.f , -0.f , +0.f ,
        +0.f , -1.f , +1.f , -2.f ,
        -0.5f, +2.5f, -0.5f, +2.5f,
    };

    const auto invertedMatrix = ts::math::inverse(matrix);
    ASSERT_EQ(ts::math::to_string(invertedMatrix), ts::math::to_string(expected));
}

TEST(MathTests, mat4rotationTest)
{
    const auto matrix = ts::math::Mat4(1.f);

    const ts::math::Vec3 axis{0.f, 1.f, 0.f};
    const auto result = ts::math::rotate(matrix, axis, ts::math::radians(180.f));

    const ts::math::Mat4 expected
    {
        -1.f, +0.f, +0.f, +0.f,
        +0.f, +1.f, +0.f, +0.f,
        +0.f, +0.f, -1.f, +0.f,
        +0.f, +0.f, +0.f, +1.f,
    };
    ASSERT_TRUE(expected[0].x == result[0].x and expected[1].y == result[1].y and expected[2].z == result[2].z);
}

class TestGame final : public ts::TesterEngine
{
    static constexpr std::chrono::steady_clock::duration renderingDuration{3s};

public:
    TestGame() : TesterEngine{renderingDuration}
    {}
};

TEST(GameTests, RunReturnsZero)
{
    const auto game = std::make_unique<TestGame>();
    const auto result = ts::run(game.get());

    ASSERT_EQ(0, result);
}

TEST(GameTests, IsThrowsException)
{
    try
    {
        const auto game = std::make_unique<TestGame>();
        const auto result = ts::run(game.get());
    }
    catch (...)
    {
        FAIL();
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}