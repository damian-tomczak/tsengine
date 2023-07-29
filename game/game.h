#pragma once

#include "tsengine/asset_store.h"
#include "tsengine/core.h"
#include "tsengine/ecs.hpp"
#include "tsengine/event_bus.hpp"

#include <iostream>
#include <memory>
#include <optional>

class Game final : public ts::Engine
{
public:
    Game() = default;

    bool init(unsigned& width, unsigned& height) override;
    void close() override;
    bool tick() override;
    void onMouseMove(int x, int y, int xrelative, int yrelative) override;
    void onMouseButtonClick(ts::MouseButton button, bool isReleased) override;
    void onKeyPressed(ts::Key k) override;
    void onKeyReleased(ts::Key k) override;

private:
    std::unique_ptr<ts::AssetStore> mpAssetStore;
    std::unique_ptr<ts::Registry> mpRegistry;
    std::unique_ptr<ts::EventBus> mpEventBus;

    std::optional<ts::Entity> mpPlayer;
};