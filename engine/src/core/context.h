#pragma once

#include "pch.h"

#include "vulkan/shaders.h"
#include "vulkan/vulkan_loader.h"
#include "openxr/openxr_platform.h"
#include "os.h"

namespace ts
{
class Context final
{
public:
    Context();

private:
    void createOpenXRInstance();
};
} // namespace ts