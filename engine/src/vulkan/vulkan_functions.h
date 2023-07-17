#pragma once

#ifdef _WIN32
#define NOMINMAX
#endif // _WIN32

#include "vulkan/vulkan.h"

#define EXPORTED_VULKAN_FUNCTION(name) extern PFN_##name name;
#define GLOBAL_LEVEL_VULKAN_FUNCTION(name) extern PFN_##name name;
#define INSTANCE_LEVEL_VULKAN_FUNCTION(name) extern PFN_##name name;
#ifndef NDEBUG
#define DEBUG_LEVEL_VULKAN_FUNCTION(name) extern PFN_##name name;
#endif // DEBUG
#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) extern PFN_##name name;
#define DEVICE_LEVEL_VULKAN_FUNCTION(name) extern PFN_##name name;
#define DEVICE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension) extern PFN_##name name;

#include "vulkan_functions.inl"