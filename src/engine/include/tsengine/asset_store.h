#pragma once

#include <iomanip>
#include <map>
#include <stdexcept>
#include <string>

namespace ts
{
class AssetStore
{
public:
    AssetStore() = default;
    virtual ~AssetStore()
    {
        clearAssets();
    };

    void clearAssets();
    void addTexture(const char* pAssetId, const char* pFilePath);
    void* getTexture(const char* pAssetId);
};
} // namespace ts
