#pragma once

namespace ts
{
inline namespace TS_VER
{
class Engine
{
public:
    Engine() = default;

    Engine(const Engine&) = delete;
    Engine& operator=(Engine&) = delete;
    Engine(const Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;

    virtual bool init(const char*& gameName, unsigned& width, unsigned& height);
    virtual void loadLvL() = 0;
    virtual bool tick(const float dt) = 0;
    virtual void close();
};

int run(Engine* const engine);
} // namespace ver
} // namespace ts

#define TS_MAIN(gameClass)                        \
    int main()                                    \
    {                                             \
        try                                       \
        {                                         \
            auto result = ts::run(new gameClass); \
            if (result != ts::TS_SUCCESS)         \
            {                                     \
                return result;                    \
            }                                     \
        }                                         \
        catch (const std::exception& e)           \
        {                                         \
            std::cerr << e.what() << "\n";        \
            return EXIT_FAILURE;                  \
        }                                         \
                                                  \
        return EXIT_SUCCESS;                      \
    }