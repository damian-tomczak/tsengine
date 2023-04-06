#include "tsengine/asset_store.h"

std::map<std::string, void*> sprites;

namespace ts
{
void AssetStore::clearAssets()
{
    //for (const auto& sprite : sprites)
    //{
    //    destroySprite(sprite.second);
    //}
    //sprites.clear();
}

void AssetStore::addTexture(const char* pAssetId, const char* pFilePath)
{
    //auto sprite{ createSprite((DATA_PATH + filePath).c_str()) };
    //if (sprite == nullptr)
    //{
    //    throw std::runtime_error{ "Creation of the sprite \"" + assetId + "\" wasn't succesful!" };
    //}
    //sprites.emplace(assetId, sprite);
}

void* AssetStore::getTexture(const char* pAssetId)
{
    //return sprites.at(assetId);
    return nullptr;
}
}