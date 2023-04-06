#pragma once

#include "tsengine/core.h"
#include "tsengine/asset_store.h"
#include "tsengine/ecs.hpp"

#include "components/transform_component.hpp"
#include "components/sprite_component.hpp"

#include <iostream>

class RenderSystem : public ts::System
{
public:
    RenderSystem()
    {
        requireComponent<TransformComponent>();
        requireComponent<SpriteComponent>();
    }

    void update(std::unique_ptr<ts::AssetStore>& pAssetStore, float deltaTime)
    {

    }
};