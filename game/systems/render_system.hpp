#pragma once

#include "tsengine/asset_store.h"
#include "tsengine/core.h"
#include "tsengine/ecs.hpp"

#include "components/texture_component.hpp"
#include "components/transform_component.hpp"

#include <iostream>

class RenderSystem : public ts::System
{
public:
    RenderSystem()
    {
        requireComponent<TransformComponent>();
        requireComponent<TextureComponent>();
    }

    void update(std::unique_ptr<ts::AssetStore>& pAssetStore, float deltaTime)
    {
        (void)pAssetStore;
        (void)deltaTime;
    }
};