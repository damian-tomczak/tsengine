#pragma once

#include "tsengine/math.hpp"

#include "tsengine/ecs/components/asset_component.h"

namespace ts
{
struct MeshComponent : public AssetComponent
{
    struct Vertex final
    {
        math::Vec3 position;
        math::Vec3 normal;
        math::Vec3 color;
    };

    size_t firstIndex{};
    size_t indexCount{};

    MeshComponent(const std::string_view fileName_ = "") : AssetComponent{fileName_}
    {}
};
}
