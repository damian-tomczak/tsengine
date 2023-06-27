#pragma once

#include "vulkan/vulkan_functions.h"

namespace vulkanloader
{
void connectWithLoader();
void loadExportFunction();
void loadGlobalLevelFunctions();
void loadInstanceLevelFunctions();
}