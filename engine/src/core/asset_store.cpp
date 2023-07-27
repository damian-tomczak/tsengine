#include "tsengine/asset_store.h"

std::map<std::string, void*> sprites;

namespace ts
{
void AssetStore::clearAssets()
{
    // for (const auto& sprite : sprites)
    //{
    //    destroySprite(sprite.second);
    //}
    // sprites.clear();
}

void AssetStore::addTexture(const char* assetId, const char* filePath)
{
    (void)assetId;
    (void)filePath;
    // auto sprite{ createSprite((DATA_PATH + filePath).c_str()) };
    // if (sprite == nullptr)
    //{
    //    throw std::runtime_error{ "Creation of the sprite \"" + assetId + "\"
    //    wasn't succesful!" };
    //}
    // sprites.emplace(assetId, sprite);
}

void* AssetStore::getTexture(const char* assetId)
{
    (void)assetId;
    // return sprites.at(assetId);
    return nullptr;
}
} // namespace ts