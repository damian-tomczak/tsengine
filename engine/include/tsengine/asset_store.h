#pragma once

#include "utils.hpp"
#include "tsengine/math.hpp"

namespace ts
{
struct Vertex final
{
    math::Vec3 position;
    math::Vec3 normal;
    math::Vec3 color;
};

class AssetStore final : public Singleton<AssetStore>
{
    SINGLETON_BODY(AssetStore);
    NOT_MOVEABLE(AssetStore);

public:
    void loadModel(const char* fileName);
};
} // namespace ts
