#include "game.h"

#include "systems/keyboard_system.hpp"
#include "systems/movement_system.hpp"
#include "systems/render_system.hpp"

#include "events/key_pressed_event.hpp"
#include "events/key_released_event.hpp"

void Game::preInit(const char* pGameName, int& width, int& height, bool& isFullscreen)
{
    pGameName = "Awesome Game!";
    width = mWidth;
    height = mHeight;
    isFullscreen = mIsFullScreen;
}

bool Game::init()
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

    mPreviousTickCount = ts::getTickCount();

    return true;
}

void Game::close()
{
}

bool Game::tick()
{
    auto currentTime{ ts::getTickCount() };
    auto deltaTime{ ((currentTime - mPreviousTickCount) / 1000.f) };
    mPreviousTickCount = currentTime;

    if (mpPlayer == std::nullopt)
    {
        mpPlayer = mpRegistry->createEntity();
        mpPlayer->setTag("player");
        mpPlayer->addComponent<TransformComponent>(
          ts::Vec2{ static_cast<float>(mWidth - (mWidth / 2)), static_cast<float>(mHeight - 50) });
        mpPlayer->addComponent<TextureComponent>("player-left");
        mpPlayer->addComponent<RigidBodyComponent>();
    }

    mpRegistry->update();

    mpRegistry->getSystem<RenderSystem>().update(mpAssetStore, deltaTime);
    mpRegistry->getSystem<MovementSystem>().update(deltaTime);

    mpRegistry->getSystem<KeyboardSystem>().subscribeToEvents(mpEventBus);

    return false;
}

void Game::onMouseMove(int x, int y, int xrelative, int yrelative)
{
}

void Game::onMouseButtonClick(ts::MouseButton button, bool isReleased)
{
}

void Game::onKeyPressed(ts::Key k)
{
    mpEventBus->emitEvent<KeyPressedEvent>(k);
}

void Game::onKeyReleased(ts::Key k)
{
    mpEventBus->emitEvent<KeyReleasedEvent>(k);
}