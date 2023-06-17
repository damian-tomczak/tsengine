#pragma once

#include "pch.h"

#include "vulkan/shaders.h"
#include "os.h"

namespace ts
{
class Context final
{
public:
    Context();

private:
    void connectWithVkLoaderLibrary();

    LIBRARY_TYPE vkLibrary{};
};
} // namespace ts