#include "game.h"

#include "systems/keyboard_system.hpp"
#include "systems/movement_system.hpp"
#include "systems/render_system.hpp"

#include "events/key_pressed_event.hpp"
#include "events/key_released_event.hpp"

#include "tsengine/math.hpp"
#include "tsengine/logger.h"

bool Game::init(unsigned& width, unsigned& height)
{
    mpAssetStore = std::make_unique<ts::AssetStore>();
    mpRegistry = std::make_unique<ts::Registry>();
    mpEventBus = std::make_unique<ts::EventBus>();

    mpAssetStore->addTexture("player-left", "lik-left.png");
    mpAssetStore->addTexture("player-right", "lik-right.png");
    mpAssetStore->addTexture("background-1", "bck.png");
    mpAssetStore->addTexture("background-2", "soccer-bck.png");

    mpRegistry->addSystem<RenderSystem>();
    mpRegistry->addSystem<KeyboardSystem>();
    mpRegistry->addSystem<MovementSystem>();

    return true;
}

void Game::close()
{}

bool Game::tick()
{
    if (mpPlayer)
    {
        mpPlayer = mpRegistry->createEntity();
        mpPlayer->setTag("player");
        mpPlayer->addComponent<TextureComponent>("player-left");
        mpPlayer->addComponent<RigidBodyComponent>();
    }

    mpRegistry->update();

    mpRegistry->getSystem<KeyboardSystem>().subscribeToEvents(mpEventBus);

    return false;
}

void Game::onMouseMove(int x, int y, int xrelative, int yrelative)
{
    (void)x;
    (void)y;
    (void)xrelative;
    (void)yrelative;
}

void Game::onMouseButtonClick(ts::MouseButton button, bool isReleased)
{
    (void)button;
    (void)isReleased;
}

void Game::onKeyPressed(ts::Key k)
{
    mpEventBus->emitEvent<KeyPressedEvent>(k);
}

void Game::onKeyReleased(ts::Key k)
{
    mpEventBus->emitEvent<KeyReleasedEvent>(k);
}

TS_MAIN()