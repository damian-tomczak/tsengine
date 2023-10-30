#pragma once

#include "tsengine/math.hpp"

struct MeshComponent
{
    const size_t assetId{};

    size_t firstIndex;
    size_t indexCount;
};
