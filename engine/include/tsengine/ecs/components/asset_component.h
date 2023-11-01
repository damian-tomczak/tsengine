#pragma once

namespace ts
{
class AssetStore;

struct AssetComponent
{
    AssetComponent(const std::string_view assetName_) : assetName{assetName_}
    {}

private:
    friend AssetStore;
    std::string assetName;
};
}