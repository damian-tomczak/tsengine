#include "vulkan_loader.h"
#include "tsengine/logger.h"
#include "os.h"

LIBRARY_TYPE vulkanLibrary{};

namespace vulkanloader
{
void connectWithLoader()
{
#ifdef WIN32
    vulkanLibrary = LoadLibrary("vulkan-1.dll");
#else
    #error "not implemented"
#endif

    if (vulkanLibrary == nullptr)
    {
        LOGGER_ERR("vulkan loader couldn't be found");
    }
}

void loadExportingFunction()
{
#define EXPORTED_VULKAN_FUNCTION(name)                                       \
    name = reinterpret_cast<PFN_##name>(LoadFunction(vulkanLibrary, #name)); \
    if(name == nullptr)                                                      \
    {                                                                        \
        LOGGER_ERR("unable to load vk export level function: " #name);       \
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
}