#pragma once

#include "utils.hpp"

namespace ts
{
class AssetStore final : public Singleton<AssetStore>
{
    SINGLETON_BODY(AssetStore);
    NOT_MOVEABLE(AssetStore);

public:
    //struct Model final
    //{
    //    size_t firstIndex;
    //    size_t indexCount;
    //};

    //void loadModel();


    //void reset();
};
} // namespace ts
