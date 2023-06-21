#pragma once

#include "vulkan/vulkan_functions.h"

namespace vulkanloader
{
void connectWithLoader();
void loadExportingFunction();
void loadGlobalLevelFunctions();
}