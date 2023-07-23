#pragma once

#include "openxr/openxr.h"
#include "vulkan/vulkan_loader.h"

#define NOT_COPYABLE(TypeName)               \
    TypeName(const TypeName&) = delete;      \
    TypeName& operator=(TypeName&) = delete;

#define NOT_MOVEABLE(TypeName)                \
    TypeName(TypeName&&) = delete;            \
    TypeName& operator=(TypeName&&) = delete;

#define NOT_COPYABLE_AND_MOVEABLE(TypeName) \
    NOT_COPYABLE(TypeName)                  \
    NOT_MOVEABLE(TypeName)

#define TS_CATCH_FALLBACK            \
    catch (const TSException&)       \
    {                                \
        return TS_FAILURE;           \
    }                                \
    catch (const std::exception& e)  \
    {                                \
        LOGGER_ERR_WO_EXC(e.what()); \
        return STL_FAILURE;          \
    }                                \
    catch (...)                      \
    {                                \
        return UNKNOWN_FAILURE;      \
    }

#define XSTR(x) #x
#define STR(x) XSTR(x)

namespace ts::utils
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

inline XrPosef makeIdentity()
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
} // namespace ts::utils
