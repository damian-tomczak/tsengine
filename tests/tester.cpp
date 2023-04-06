#include "gtest/gtest.h"

TEST(DummyTests, Dummytest)
{
    const auto multiplication = 28*10*2000;
    const auto expected = 560000;
    ASSERT_EQ(expected, multiplication);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}