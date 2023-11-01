#pragma once

#include "utils.hpp"

namespace ts
{
class AssetStore final : Singleton<AssetStore>
{
    SINGLETON_BODY(AssetStore);

    class Models
    {
        static void load();
        static void writeTo(char* const destination);

        size_t getIndexOffset();
        size_t getSize();
    };
};
} // namespace ts
