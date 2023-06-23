#pragma once

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

    virtual void preInit(const char*& pGameName, int& width, int& height, bool& isFullscreen) = 0;
    // TODO: reconsider if init is needed
    virtual bool init() = 0;
    virtual void close() = 0;
    virtual bool tick() = 0;

    virtual void onMouseMove(int x, int y, int xrelative, int yrelative) = 0;
    virtual void onMouseButtonClick(MouseButton button, bool isReleased) = 0;
    virtual void onKeyPressed(Key k) = 0;
    virtual void onKeyReleased(Key k) = 0;
};

int run(Engine* const pEngine);
}
