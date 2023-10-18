#pragma once

#include "internal_utils.h"
#include "tsengine/logger.h"
#include "tsengine/math.hpp"

#include "openxr/openxr.h"
#include "vulkan/vulkan.h"

#define LOGGER_VK(function, ...)                                                                           \
    {                                                                                                      \
        VkResult result{function(__VA_ARGS__)};                                                            \
        if (result != VK_SUCCESS)                                                                          \
        {                                                                                                  \
            ts::logger::error(                                                                             \
                (#function " failed with status: " + ts::khronos_utils::vkResultToString(result)).c_str(), \
                __FILE__,                                                                                  \
                FUNCTION_SIGNATURE,                                                                        \
                __LINE__);                                                                                 \
        }                                                                                                  \
    }

#define LOGGER_XR(function, ...)                                                                           \
    {                                                                                                      \
        XrResult result{function(__VA_ARGS__)};                                                            \
        if (result != XR_SUCCESS)                                                                          \
        {                                                                                                  \
            ts::logger::error(                                                                             \
                (#function " failed with status: " + ts::khronos_utils::xrResultToString(result)).c_str(), \
                __FILE__,                                                                                  \
                FUNCTION_SIGNATURE,                                                                        \
                __LINE__);                                                                                 \
        }                                                                                                  \
    }


namespace ts::khronos_utils
{
inline constexpr std::array knownXrDevices
{
    XrSystemId{0x14},
    XrSystemId{0x1000'D8F4'0000'0268}, 
    XrSystemId{0x1000'38B4'B789'18E8},
};

inline constexpr std::array knownUnstableXrDevices
{
    XrSystemId{0x1000'D8F4'0000'0268},
    XrSystemId{0x1000'38B4'B789'18E8},
};

std::string vkResultToString(const VkResult result);
std::string xrResultToString(const XrResult result);

#ifndef NDEBUG
constexpr XrDebugUtilsMessageSeverityFlagsEXT xrDebugMessageSeverityFlags =
    XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
    XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

constexpr XrDebugUtilsMessageTypeFlagsEXT xrDebugMessageTypeFlags =
    XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
    XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_CONFORMANCE_BIT_EXT;

XrBool32 xrCallback(
    XrDebugUtilsMessageSeverityFlagsEXT messageSeverity,
    XrDebugUtilsMessageTypeFlagsEXT messageTypes,
    const XrDebugUtilsMessengerCallbackDataEXT* callbackData,
    void* userData);

constexpr VkFlags64 vkDebugMessageSeverityFlags =
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

constexpr VkFlags64 vkDebugMessageTypeFlags =
    VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
    VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

VkBool32 vkCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void* userData);
#endif // !NDEBUG

// TODO: investigate performance of it
void unpackXrExtensionString(const std::string& str, std::vector<std::string>& result);
XrPosef makeXrIdentity();
bool findSuitableMemoryTypeIndex(
    VkPhysicalDevice pPhysicalDevice,
    VkMemoryRequirements pRequirements,
    VkMemoryPropertyFlags pProperties,
    uint32_t& typeIndex);
VkDeviceSize align(const VkDeviceSize value, const VkDeviceSize alignment);
math::Mat4 createXrProjectionMatrix(const XrFovf fov, const float nearClip, const float farClip);
math::Mat4 xrPoseToMatrix(const XrPosef& pose);
} // namespace ts::utils