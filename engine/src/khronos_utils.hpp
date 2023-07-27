#pragma once

#include "tsengine/math.hpp"

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

inline math::Mat4 createXrProjectionMatrix(const XrFovf fov, const float nearClip, const float farClip)
{
    const auto l = tan(fov.angleLeft);
    const auto r = tan(fov.angleRight);
    const auto d = tan(fov.angleDown);
    const auto u = tan(fov.angleUp);

    const auto w = r - l;
    const auto h = d - u;

    math::Mat4 projectionMatrix{{{
        2.0f / w   , 0.0f       , 0.0f                                                     , 0.0f ,
        0.0f       , 2.0f / h   , 0.0f                                                     , 0.0f ,
        (r + l) / w, (u + d) / h, -(farClip + nearClip) / (farClip - nearClip)             , -1.0f,
        0.0f       , 0.0f       , -(farClip * (nearClip + nearClip)) / (farClip - nearClip), 0.0f ,
    }}};

    return projectionMatrix;
}

inline math::Mat4 xrPoseToMatrix(const XrPosef& pose)
{
    const auto translation =
        math::translate(math::Mat4::makeScalarMat(1.f), {pose.position.x, pose.position.y, pose.position.z});

    const auto rotation =
        math::Mat4(math::Quat(pose.orientation.w, pose.orientation.x, pose.orientation.y, pose.orientation.z));

    return translation * rotation;
}
} // namespace ts::utils