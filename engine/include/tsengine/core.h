#pragma once

namespace ts
{
class Engine
{
public:
    Engine() = default;

    Engine(const Engine&) = delete;
    Engine& operator=(Engine&) = delete;
    Engine(const Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;

    virtual bool init(const char*& gameName, unsigned& width, unsigned& height) = 0;
    virtual void loadLvL() {};
    virtual bool tick() = 0;
    virtual void close() = 0;
};

int run(Engine* const engine);
} // namespace ts

#define TS_MAIN()                            \
    int main()                               \
    {                                        \
        try                                  \
        {                                    \
            auto result = ts::run(new Game); \
            if (result != ts::TS_SUCCESS)    \
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