#pragma once

#include <string>

struct TextureComponent
{
    std::string assetId;

    TextureComponent(std::string assetId_) : assetId{ assetId_ }
    {
    }
};