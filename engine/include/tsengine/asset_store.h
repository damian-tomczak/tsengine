#pragma once

#include "utils.hpp"

#include "tsengine/ecs/ecs.hpp"
#include "tsengine/ecs/components/mesh_component.hpp"

namespace ts
{
class AssetStore final : public System, public Singleton<AssetStore>
{
    NOT_COPYABLE(AssetStore);
    friend Singleton<AssetStore>;

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
} // namespace ts
