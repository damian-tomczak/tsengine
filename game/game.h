#pragma once

#include "tsengine/asset_store.h"
#include "tsengine/core.h"
#include "tsengine/ecs/ecs.h"
#include "tsengine/event_bus.hpp"

#include <iostream>
#include <memory>
#include <optional>

class Game final : public ts::Engine
{
public:
    Game() = default;

    bool init(const char*& gameName, unsigned& width, unsigned& height) override;
    void loadLvL() override;
    bool tick() override;
    void close() override;
};