#include "vulkan_loader.h"
#include "tsengine/logger.h"
#include "os.h"

LIBRARY_TYPE vulkanLibrary{};

namespace vulkanloader
{
void connectWithLoader()
{
#ifdef _WIN32
    vulkanLibrary = LoadLibrary("vulkan-1.dll");
#else
#error "not implemented"
#endif // _WIN32

    if (vulkanLibrary == nullptr)
    {
        LOGGER_ERR("vulkan loader couldn't be found");
    }
}

void loadExportFunction()
{
#define EXPORTED_VULKAN_FUNCTION(name)                                       \
    name = reinterpret_cast<PFN_##name>(LoadFunction(vulkanLibrary, #name)); \
    if(name == nullptr)                                                      \
    {                                                                        \
        LOGGER_ERR("unable to load vk export function: " #name);             \
    }

#include "vulkan_functions.inl"
}

void loadGlobalLevelFunctions()
{
#define GLOBAL_LEVEL_VULKAN_FUNCTION(name)                                      \
    name = reinterpret_cast<PFN_##name>(vkGetInstanceProcAddr(nullptr, #name)); \
    if (name == nullptr)                                                        \
    {                                                                           \
        LOGGER_ERR("unable to load vk global level function: " #name);          \
    }

#include "vulkan_functions.inl"
}

void loadInstanceLevelFunctions(const VkInstance instance, const std::vector<std::string>& vulkanInstanceExtensions)
{
#define INSTANCE_LEVEL_VULKAN_FUNCTION(name)                                         \
        name = reinterpret_cast<PFN_##name>(vkGetInstanceProcAddr(instance, #name)); \
        if(name == nullptr)                                                          \
        {                                                                            \
            LOGGER_ERR("unable to load vk instance level function: " #name);         \
        }

#define INSTANCE_LEVEL_VULKAN_FUNCTION_FROM_EXTENSION(name, extension)                              \
        for (const auto& enabledExtension : vulkanInstanceExtensions)                               \
        {                                                                                           \
            if (strcmp(enabledExtension.c_str(), extension) == 0)                                   \
            {                                                                                       \
                name = reinterpret_cast<PFN_##name>(vkGetInstanceProcAddr(instance, #name));        \
                if (name == nullptr)                                                                \
                {                                                                                   \
                    LOGGER_ERR("unable to load vk instance extension level function: " #extension); \
                }                                                                                   \
            }                                                                                       \
        }

#include "vulkan_functions.inl"
}

#ifdef _DEBUG
void loadDebugLevelFunctions(const VkInstance instance)
{
#define DEBUG_LEVEL_VULKAN_FUNCTION(name)                                               \
    name = reinterpret_cast<PFN_##name>(vkGetInstanceProcAddr(instance, #name));        \
    if(name == nullptr)                                                                 \
    {                                                                                   \
        LOGGER_ERR("unable to load vk instance extension level function: " #name);      \
    }

#include "vulkan_functions.inl"
}
#endif
}