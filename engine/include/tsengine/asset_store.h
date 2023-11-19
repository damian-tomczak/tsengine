#pragma once

#include "utils.hpp"

#include "tsengine/ecs/ecs.h"
#include "tsengine/ecs/components/mesh_component.hpp"

namespace ts
{
inline namespace TS_VER
{
class AssetStore final : public System
{
public:
    AssetStore()
    {
        requireComponent<MeshComponent>();
    }

    struct Models
    {
        static void load();
        static void writeTo(char* const destination);

        static size_t getIndexOffset();
        static size_t getSize();
    };
};
} // namespace ver
} // namespace ts
