#pragma once

#include "utils.hpp"
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
using XrDeviceId = size_t;
namespace device_names
{
    inline constexpr std::string_view HtcVivePro{"Vive OpenXR: Vive SRanipal"};
    inline constexpr std::string_view OculusQuest2{"Oculus Quest2"};
} // namespace device_ids
namespace device_ids
{
    inline constexpr auto HtcVivePro = std::hash<std::string_view>{}(device_names::HtcVivePro);
    inline constexpr auto OcuslusQuest2 = std::hash<std::string_view>{}(device_names::OculusQuest2);
} // namespace device_ids
constexpr std::array knownXrDevicesNameToId{
    std::pair<std::string_view, XrDeviceId>{device_names::HtcVivePro, device_ids::HtcVivePro},
    std::pair<std::string_view, XrDeviceId>{device_names::OculusQuest2, device_ids::OcuslusQuest2},
};
constexpr std::array knownXrDevicesIdToName{
    std::pair<XrDeviceId, std::string_view>{device_ids::HtcVivePro, device_names::HtcVivePro},
    std::pair<XrDeviceId, std::string_view>{device_ids::OcuslusQuest2, device_names::OculusQuest2},
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
VkDeviceSize align(VkDeviceSize value, VkDeviceSize alignment);
math::Mat4 createXrProjectionMatrix(const XrFovf fov, const float nearClip, const float farClip);
math::Mat4 xrPoseToMatrix(const XrPosef& pose);
} // namespace ts::utils