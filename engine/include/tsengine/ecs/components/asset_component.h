#pragma once

#include "tsengine/utils.hpp"

namespace ts
{
inline namespace TS_VER
{
class AssetStore;

struct AssetComponent : public Component
{
    using Base = AssetComponent;

    AssetComponent(const std::string_view assetName_ = "") : assetName{assetName_}, assetNameId{std::hash<std::string_view>{}(assetName_)}
    {}

protected:
    friend AssetStore;
    std::string assetName;
    size_t assetNameId;
};
} // namespace ver
} // namespace ts