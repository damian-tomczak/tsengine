#pragma once

namespace ts
{
// Get the number of milliseconds since engine initialization.
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
    virtual ~Engine() = default;

    Engine(const Engine&) = delete;
    Engine& operator=(Engine&) = delete;

    Engine(const Engine&&) = delete;
    Engine& operator=(Engine&&) = delete;

    // no function calls are available here, this function should only return
    // width, height and fullscreen values
    virtual void preInit(const char*& pGameName, int& width, int& height, bool& isFullscreen) = 0;

    // return : true - ok, false - failed, application will exit
    virtual bool init() = 0;

    virtual void close() = 0;

    // return value: if true will exit the application
    virtual bool tick() = 0;

    // param: xrel, yrel: The relative motion in the X/Y direction
    // param: x, y : coordinate, relative to window
    virtual void onMouseMove(int x, int y, int xrelative, int yrelative) = 0;

    virtual void onMouseButtonClick(MouseButton button, bool isReleased) = 0;

    virtual void onKeyPressed(Key k) = 0;

    virtual void onKeyReleased(Key k) = 0;
};

int run(Engine* pEngine);
} // namespace ts
