#pragma once

#include <map>
#include <string>
#include <stdexcept>
#include <iomanip>

namespace ts
{
class AssetStore
{
public:
    AssetStore() = default;
    virtual ~AssetStore() { clearAssets(); };

    void clearAssets();
    void addTexture(const char* pAssetId, const char* pFilePath);
    void* getTexture(const char* pAssetId);

};
}
