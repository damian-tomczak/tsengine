#include "gtest/gtest.h"
#include "tsengine/core.h"
#include <memory>

TEST(DummyTests, Dummytest)
{
    const auto multiplication = 28 * 10 * 2000;
    const auto expected = 560000;
    ASSERT_EQ(expected, multiplication);
}

class Game final : public ts::Engine
{
public:
    Game() = default;

    void preInit(const char*& pGameName, int& width, int& height, bool& isFullscreen) override 
    {
        pGameName = "Awesome Game!";
        width = 1280;
        height = 720;
        isFullscreen = true;
    }

    bool init() override { return true; }

    void close() override
    {}

    bool tick() override { return true; }

    void onMouseMove(int x, int y, int xrelative, int yrelative) override
    {}

    void onMouseButtonClick(ts::MouseButton button, bool isReleased) override
    {}

    void onKeyPressed(ts::Key k) override
    {}

    void onKeyReleased(ts::Key k) override
    {}
};

TEST(GameTests, RunReturnsZero)
{
    auto result{ ts::run(new Game) };

    ASSERT_EQ(0, result);
}

TEST(GameTests, IsThrowsException)
{
    try
    {
        std::unique_ptr<Game> pGame{ new Game };
        auto result{ ts::run(pGame.get()) };
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