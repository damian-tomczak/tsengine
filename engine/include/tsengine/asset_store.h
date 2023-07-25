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
    virtual ~AssetStore() { clearAssets(); };

    void clearAssets();
    void addTexture(const char* assetId, const char* filePath);
    void* getTexture(const char* assetId);
};
} // namespace ts
