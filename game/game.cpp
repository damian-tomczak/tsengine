#include "game.h"

#include "tsengine/math.hpp"
#include "tsengine/logger.h"

#include "tsengine/ecs/ecs.hpp"
#include "tsengine/ecs/components/transform_component.hpp"
#include "tsengine/ecs/components/rigid_body_component.hpp"

bool Game::init(const char*& gameName, unsigned&, unsigned&)
{
    gameName = GAME_NAME;

    return true;
}

void Game::loadLvl()
{
    constexpr size_t spheresCount = 3;

    for (size_t i{}; i < spheresCount; ++i)
    {

    }
}

bool Game::tick()
{
    return false;
}

void Game::close()
{

}

TS_MAIN()