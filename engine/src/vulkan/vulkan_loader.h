#pragma once

#include "vulkan/vulkan_functions.h"

namespace vulkanloader
{
void connectWithLoader();
void loadExportFunction();
void loadGlobalLevelFunctions();
void loadInstanceLevelFunctions(const VkInstance instance, const std::vector<std::string>& vulkanInstanceExtensions);
void loadDebugLevelFunctions(const VkInstance instance);
}