#include "game.h"

#include "tsengine/math.hpp"
#include "tsengine/logger.h"

#include "tsengine/ecs/ecs.hpp"
#include "tsengine/ecs/components/transform_component.hpp"
#include "tsengine/ecs/components/rigid_body_component.hpp"
#include "tsengine/ecs/components/renderer_component.hpp"

bool Game::init(const char*& gameName, unsigned&, unsigned&)
{
    gameName = GAME_NAME;

    return true;
}

void Game::loadLvl()
{
    ts::Entity ruins = ts::gRegistry.createEntity();
    ruins.addComponent<ts::TransformComponent>();
    ruins.addComponent<ts::RendererComponent<ts::PipelineType::NORMAL_LIGHTING>>();

    ts::Entity polonez = ts::gRegistry.createEntity();
    polonez.addComponent<ts::TransformComponent>(ts::math::Vec3{0.f, 0.f, -10.f});
    polonez.addComponent<ts::RendererComponent<ts::PipelineType::NORMAL_LIGHTING>>();

    constexpr size_t spheresCount = 3;

    for (size_t i{}; i < spheresCount; ++i)
    {
        ts::Entity sphere = ts::gRegistry.createEntity();
        sphere.addComponent<ts::TransformComponent>(ts::math::Vec3{spheresCount / 3 * -5.f + 5.f * i, 2.f, -5.f});
        sphere.addComponent<ts::RendererComponent<ts::PipelineType::PBR>>();
    }

    ts::Entity light1 = ts::gRegistry.createEntity();
    light1.addComponent<ts::TransformComponent>(ts::math::Vec3{0.f, 5.f, -7.f});
    light1.addComponent<ts::RendererComponent<ts::PipelineType::LIGHT>>();

    ts::Entity light2 = ts::gRegistry.createEntity();
    light2.addComponent<ts::TransformComponent>(ts::math::Vec3{0.f, 5.f,  0.f});
    light2.addComponent<ts::RendererComponent<ts::PipelineType::LIGHT>>();
}

bool Game::tick()
{
    return false;
}

void Game::close()
{

}

TS_MAIN()