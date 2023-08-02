#pragma once

#define TS_MAIN()                            \
    int main()                               \
    {                                        \
        try                                  \
        {                                    \
            auto result = ts::run(new Game); \
            if (result != ts::SUCCESS)       \
            {                                \
                return result;               \
            }                                \
        }                                    \
        catch (const std::exception& e)      \
        {                                    \
            std::cerr << e.what() << "\n";   \
            return EXIT_FAILURE;             \
        }                                    \
                                             \
        return EXIT_SUCCESS;                 \
    }

namespace ts
{
unsigned getTickCount();

enum class Key
{
    RIGHT,
    LEFT,
    DOWN,
    UP
};

enum class MouseButton
{
    LEFT,
    MIDDLE,
    RIGHT
};

class Engine
{
public:
    Engine() = default;

    Engine(const Engine&) = delete;
    Engine& operator=(Engine&) = delete;
    Engine(const Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;

    virtual bool init(unsigned& width, unsigned& height) = 0;
    virtual void close() = 0;
    virtual bool tick() = 0;

    virtual void onMouseMove(int x, int y, int xrelative, int yrelative) = 0;
    virtual void onMouseButtonClick(MouseButton button, bool isReleased) = 0;
    virtual void onKeyPressed(Key k) = 0;
    virtual void onKeyReleased(Key k) = 0;
};

int run(Engine* engine);
} // namespace ts
