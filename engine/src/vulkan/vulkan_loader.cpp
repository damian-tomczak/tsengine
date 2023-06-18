#include "vulkan_loader.h"

#include "os.h"

LIBRARY_TYPE vulkanLibrary{};

namespace vulkanloader
{
void connectWithLoader()
{
    vulkanLibrary = LoadLibrary("vulkan-1.dll");

    if (vulkanLibrary == nullptr)
    {
        // TODO: logger
    }
}

void loadExportingFunction()
{
#define EXPORTED_VULKAN_FUNCTION(name)                                       \
    name = reinterpret_cast<PFN_##name>(LoadFunction(vulkanLibrary, #name)); \
    if(name == nullptr)                                                      \
    {                                                                        \
        /* TODO: logger */                                                   \
    }

#include "vulkan_functions.inl"
}

void loadGlobalLevelFunctions()
{
#define GLOBAL_LEVEL_VULKAN_FUNCTION(name)                                      \
    name = reinterpret_cast<PFN_##name>(vkGetInstanceProcAddr(nullptr, #name)); \
    if (name == nullptr)                                                        \
    {                                                                           \
        /* TODO: logger */                                                      \
    }

#include "vulkan_functions.inl"
}
}