#include "game.h"

#include "systems/keyboard_system.hpp"

#include "events/key_pressed_event.hpp"
#include "events/key_released_event.hpp"

#include "tsengine/math.hpp"
#include "tsengine/logger.h"

bool Game::init(const char*& gameName, unsigned& width, unsigned& height) { return true; }

void Game::close()
{}

bool Game::tick() { return false; }

void Game::onMouseMove(int, int, int, int)
{}

void Game::onMouseButtonClick(ts::MouseButton, bool)
{}

void Game::onKeyPressed(ts::Key)
{}

void Game::onKeyReleased(ts::Key)
{}

TS_MAIN()