#pragma once

namespace ts
{
class AssetStore;

struct AssetComponent : public Component
{
    using Base = AssetComponent;

    AssetComponent(const std::string_view assetName_ = "") : assetName{assetName_}
    {}

protected:
    friend AssetStore;
    std::string assetName;
};
}