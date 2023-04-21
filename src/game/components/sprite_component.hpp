#pragma once

#include <string>

struct SpriteComponent
{
    std::string assetId;

    SpriteComponent(std::string assetId_) : assetId{ assetId_ }
    {
    }
};