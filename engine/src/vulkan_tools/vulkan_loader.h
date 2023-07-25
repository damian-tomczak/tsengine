#pragma once

#include "vulkan_functions.h"

namespace vulkanloader
{
void connectWithLoader();
void loadExportFunction();
void loadGlobalLevelFunctions();
void loadInstanceLevelFunctions(const VkInstance pInstance, const std::vector<std::string>& vulkanInstanceExtensions);
void loadDebugLevelFunctions(const VkInstance pInstance);
void loadDeviceLevelFunctions(const VkDevice vkDevice, const std::vector<std::string>& enabledVulkanDeviceExtensions);
}