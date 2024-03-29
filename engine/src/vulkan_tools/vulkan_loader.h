#pragma once

#include "vulkan_functions.h"

namespace ts
{
inline namespace TS_VER
{
namespace vkLoader
{
void connectWithLoader();
void loadExportFunction();
void loadGlobalLevelFunctions();
void loadInstanceLevelFunctions(const VkInstance instance, const std::vector<std::string>& vulkanInstanceExtensions);
void loadDebugLevelFunctions(const VkInstance instance);
void loadDeviceLevelFunctions(const VkDevice device, const std::vector<std::string>& enabledVulkanDeviceExtensions);
} // namespace vkLoader
} // namespace ver
} // namespace ts