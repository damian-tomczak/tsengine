#pragma once

#include "openxr/openxr.h"
#include "vulkan_tools/vulkan_loader.h"

namespace ts::khronos_utils
{
// TODO: investigate performance of it
inline void unpackXrExtensionString(const std::string& str, std::vector<std::string>& result)
{
    std::istringstream stream(str);
    std::string extension;

    while (getline(stream, extension, ' '))
    {
        result.emplace_back(std::move(extension));
    }
}

inline XrPosef makeXrIdentity()
{
    const XrPosef identity2{
        .orientation = {0.0f, 0.0f, 0.0f, 1.0f},
        .position = {0.0f, 0.0f, 0.0f},
    };

    return identity2;
}

inline bool findSuitableMemoryTypeIndex(
    VkPhysicalDevice pPhysicalDevice,
    VkMemoryRequirements pRequirements,
    VkMemoryPropertyFlags pProperties,
    uint32_t& typeIndex)
{
    VkPhysicalDeviceMemoryProperties supportedMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(pPhysicalDevice, &supportedMemoryProperties);

    const VkMemoryPropertyFlags typeFilter{pRequirements.memoryTypeBits};
    for (uint32_t memoryTypeIndex{}; memoryTypeIndex < supportedMemoryProperties.memoryTypeCount; ++memoryTypeIndex)
    {
        const VkMemoryPropertyFlags propertyFlags{supportedMemoryProperties.memoryTypes[memoryTypeIndex].propertyFlags};
        if ((typeFilter & (1 << memoryTypeIndex)) && ((propertyFlags & pProperties) == pProperties))
        {
            typeIndex = memoryTypeIndex;
            return true;
        }
    }

    return false;
}

inline VkDeviceSize align(VkDeviceSize value, VkDeviceSize alignment)
{
    if (value == 0u)
    {
        return value;
    }

    return (value + alignment - 1u) & ~(alignment - 1u);
}
} // namespace ts::utils