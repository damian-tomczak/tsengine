#include "game.h"

#include "tsengine/math.hpp"
#include "tsengine/logger.h"

#include "tsengine/ecs/ecs.hpp"
#include "tsengine/ecs/components/transform_component.hpp"
#include "tsengine/ecs/components/rigid_body_component.hpp"
#include "tsengine/ecs/components/renderer_component.hpp"
#include "tsengine/ecs/components/mesh_component.hpp"

bool Game::init(const char*& gameName, unsigned&, unsigned&)
{
    gameName = GAME_NAME;

    return true;
}

void Game::loadLvL()
{
    ts::Entity village = ts::gRegistry.createEntity();
    village.tag("village");
    village.addComponent<ts::TransformComponent>();
    village.addComponent<ts::RendererComponent<ts::PipelineType::NORMAL_LIGHTING>>();
    village.addComponent<ts::MeshComponent>("assets/models/village.obj");

    ts::Entity polonez = ts::gRegistry.createEntity();
    polonez.tag("polonez");
    polonez.addComponent<ts::TransformComponent>(ts::math::Vec3{0.f, 0.f, -10.f});
    polonez.addComponent<ts::RendererComponent<ts::PipelineType::NORMAL_LIGHTING>>();
    polonez.addComponent<ts::MeshComponent>("assets/models/polonez.obj");

    static constexpr size_t spheresNumber = 3;

    for (size_t i{}; i < spheresNumber; ++i)
    {
        ts::Entity sphere = ts::gRegistry.createEntity();
        sphere.tag("sphere" + std::to_string(i));
        sphere.addComponent<ts::TransformComponent>(ts::math::Vec3{spheresNumber / 3 * -5.f + 5.f * i, 2.f, -5.f});
        sphere.addComponent<ts::RendererComponent<ts::PipelineType::PBR>>();
        sphere.addComponent<ts::MeshComponent>("assets/models/sphere.obj");
    }

    ts::Entity light1 = ts::gRegistry.createEntity();
    light1.tag("light1");
    light1.addComponent<ts::TransformComponent>(ts::math::Vec3{0.f, 5.f, -7.f});
    light1.addComponent<ts::RendererComponent<ts::PipelineType::LIGHT>>();

    ts::Entity light2 = ts::gRegistry.createEntity();
    light2.tag("light2");
    light2.addComponent<ts::TransformComponent>(ts::math::Vec3{0.f, 5.f,  0.f});
    light2.addComponent<ts::RendererComponent<ts::PipelineType::LIGHT>>();
}

bool Game::tick()
{
    return true;
}

void Game::close()
{

}

TS_MAIN()