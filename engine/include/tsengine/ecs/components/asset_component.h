#pragma once

#include "tsengine/utils.hpp"

namespace ts
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
}