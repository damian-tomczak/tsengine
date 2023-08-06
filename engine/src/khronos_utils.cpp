#include "khronos_utils.h"
#include "vulkan_tools/vulkan_functions.h"
#include "globals.hpp"

#undef STR

namespace ts::khronos_utils
{
std::string vkResultToString(const VkResult result)
{
    switch (result)
    {
#define STR(r) case VK_##r: return #r
        STR(SUCCESS);
        STR(NOT_READY);
        STR(TIMEOUT);
        STR(EVENT_SET);
        STR(EVENT_RESET);
        STR(INCOMPLETE);
        STR(ERROR_OUT_OF_HOST_MEMORY);
        STR(ERROR_OUT_OF_DEVICE_MEMORY);
        STR(ERROR_INITIALIZATION_FAILED);
        STR(ERROR_DEVICE_LOST);
        STR(ERROR_MEMORY_MAP_FAILED);
        STR(ERROR_LAYER_NOT_PRESENT);
        STR(ERROR_EXTENSION_NOT_PRESENT);
        STR(ERROR_FEATURE_NOT_PRESENT);
        STR(ERROR_INCOMPATIBLE_DRIVER);
        STR(ERROR_TOO_MANY_OBJECTS);
        STR(ERROR_FORMAT_NOT_SUPPORTED);
        STR(ERROR_FRAGMENTED_POOL);
        STR(ERROR_UNKNOWN);
        STR(ERROR_OUT_OF_POOL_MEMORY);
        STR(ERROR_INVALID_EXTERNAL_HANDLE);
        STR(ERROR_FRAGMENTATION);
        STR(ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
        STR(PIPELINE_COMPILE_REQUIRED);
        STR(ERROR_SURFACE_LOST_KHR);
        STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
        STR(SUBOPTIMAL_KHR);
        STR(ERROR_OUT_OF_DATE_KHR);
        STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
        STR(ERROR_VALIDATION_FAILED_EXT);
        STR(ERROR_INVALID_SHADER_NV);
        STR(ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR);
        STR(ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR);
        STR(ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR);
        STR(ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR);
        STR(ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR);
        STR(ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR);
        STR(ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
        STR(ERROR_NOT_PERMITTED_KHR);
        STR(ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
        STR(THREAD_IDLE_KHR);
        STR(THREAD_DONE_KHR);
        STR(OPERATION_DEFERRED_KHR);
        STR(OPERATION_NOT_DEFERRED_KHR);
        STR(ERROR_COMPRESSION_EXHAUSTED_EXT);
        STR(ERROR_INCOMPATIBLE_SHADER_BINARY_EXT);
#undef STR
    default:
        return "VK_RESULT_PARSING_UNKNOWN";
    }
}

std::string xrResultToString(const XrResult result)
{
    switch (result)
    {
#define STR(r) case XR_##r: return #r
        STR(TIMEOUT_EXPIRED);
        STR(SESSION_LOSS_PENDING);
        STR(EVENT_UNAVAILABLE);
        STR(SPACE_BOUNDS_UNAVAILABLE);
        STR(SESSION_NOT_FOCUSED);
        STR(FRAME_DISCARDED);
        STR(ERROR_VALIDATION_FAILURE);
        STR(ERROR_RUNTIME_FAILURE);
        STR(ERROR_OUT_OF_MEMORY);
        STR(ERROR_API_VERSION_UNSUPPORTED);
        STR(ERROR_INITIALIZATION_FAILED);
        STR(ERROR_FUNCTION_UNSUPPORTED);
        STR(ERROR_FEATURE_UNSUPPORTED);
        STR(ERROR_EXTENSION_NOT_PRESENT);
        STR(ERROR_LIMIT_REACHED);
        STR(ERROR_SIZE_INSUFFICIENT);
        STR(ERROR_HANDLE_INVALID);
        STR(ERROR_INSTANCE_LOST);
        STR(ERROR_SESSION_RUNNING);
        STR(ERROR_SESSION_NOT_RUNNING);
        STR(ERROR_SESSION_LOST);
        STR(ERROR_SYSTEM_INVALID);
        STR(ERROR_PATH_INVALID);
        STR(ERROR_PATH_COUNT_EXCEEDED);
        STR(ERROR_PATH_FORMAT_INVALID);
        STR(ERROR_PATH_UNSUPPORTED);
        STR(ERROR_LAYER_INVALID);
        STR(ERROR_LAYER_LIMIT_EXCEEDED);
        STR(ERROR_SWAPCHAIN_RECT_INVALID);
        STR(ERROR_SWAPCHAIN_FORMAT_UNSUPPORTED);
        STR(ERROR_ACTION_TYPE_MISMATCH);
        STR(ERROR_SESSION_NOT_READY);
        STR(ERROR_SESSION_NOT_STOPPING);
        STR(ERROR_TIME_INVALID);
        STR(ERROR_REFERENCE_SPACE_UNSUPPORTED);
        STR(ERROR_FILE_ACCESS_ERROR);
        STR(ERROR_FILE_CONTENTS_INVALID);
        STR(ERROR_FORM_FACTOR_UNSUPPORTED);
        STR(ERROR_FORM_FACTOR_UNAVAILABLE);
        STR(ERROR_API_LAYER_NOT_PRESENT);
        STR(ERROR_CALL_ORDER_INVALID);
        STR(ERROR_GRAPHICS_DEVICE_INVALID);
        STR(ERROR_POSE_INVALID);
        STR(ERROR_INDEX_OUT_OF_RANGE);
        STR(ERROR_VIEW_CONFIGURATION_TYPE_UNSUPPORTED);
        STR(ERROR_ENVIRONMENT_BLEND_MODE_UNSUPPORTED);
        STR(ERROR_NAME_DUPLICATED);
        STR(ERROR_NAME_INVALID);
        STR(ERROR_ACTIONSET_NOT_ATTACHED);
        STR(ERROR_ACTIONSETS_ALREADY_ATTACHED);
        STR(ERROR_LOCALIZED_NAME_DUPLICATED);
        STR(ERROR_LOCALIZED_NAME_INVALID);
        STR(ERROR_GRAPHICS_REQUIREMENTS_CALL_MISSING);
        STR(ERROR_RUNTIME_UNAVAILABLE);
        STR(ERROR_ANDROID_THREAD_SETTINGS_ID_INVALID_KHR);
        STR(ERROR_ANDROID_THREAD_SETTINGS_FAILURE_KHR);
        STR(ERROR_CREATE_SPATIAL_ANCHOR_FAILED_MSFT);
        STR(ERROR_SECONDARY_VIEW_CONFIGURATION_TYPE_NOT_ENABLED_MSFT);
        STR(ERROR_CONTROLLER_MODEL_KEY_INVALID_MSFT);
        STR(ERROR_REPROJECTION_MODE_UNSUPPORTED_MSFT);
        STR(ERROR_COMPUTE_NEW_SCENE_NOT_COMPLETED_MSFT);
        STR(ERROR_SCENE_COMPONENT_ID_INVALID_MSFT);
        STR(ERROR_SCENE_COMPONENT_TYPE_MISMATCH_MSFT);
        STR(ERROR_SCENE_MESH_BUFFER_ID_INVALID_MSFT);
        STR(ERROR_SCENE_COMPUTE_FEATURE_INCOMPATIBLE_MSFT);
        STR(ERROR_SCENE_COMPUTE_CONSISTENCY_MISMATCH_MSFT);
        STR(ERROR_DISPLAY_REFRESH_RATE_UNSUPPORTED_FB);
        STR(ERROR_COLOR_SPACE_UNSUPPORTED_FB);
        STR(ERROR_SPACE_COMPONENT_NOT_SUPPORTED_FB);
        STR(ERROR_SPACE_COMPONENT_NOT_ENABLED_FB);
        STR(ERROR_SPACE_COMPONENT_STATUS_PENDING_FB);
        STR(ERROR_SPACE_COMPONENT_STATUS_ALREADY_SET_FB);
        STR(ERROR_UNEXPECTED_STATE_PASSTHROUGH_FB);
        STR(ERROR_FEATURE_ALREADY_CREATED_PASSTHROUGH_FB);
        STR(ERROR_FEATURE_REQUIRED_PASSTHROUGH_FB);
        STR(ERROR_NOT_PERMITTED_PASSTHROUGH_FB);
        STR(ERROR_INSUFFICIENT_RESOURCES_PASSTHROUGH_FB);
        STR(ERROR_UNKNOWN_PASSTHROUGH_FB);
        STR(ERROR_RENDER_MODEL_KEY_INVALID_FB);
        STR(RENDER_MODEL_UNAVAILABLE_FB);
        STR(ERROR_MARKER_NOT_TRACKED_VARJO);
        STR(ERROR_MARKER_ID_INVALID_VARJO);
        STR(ERROR_SPATIAL_ANCHOR_NAME_NOT_FOUND_MSFT);
        STR(ERROR_SPATIAL_ANCHOR_NAME_INVALID_MSFT);
        STR(ERROR_SPACE_MAPPING_INSUFFICIENT_FB);
        STR(ERROR_SPACE_LOCALIZATION_FAILED_FB);
        STR(ERROR_SPACE_NETWORK_TIMEOUT_FB);
        STR(ERROR_SPACE_NETWORK_REQUEST_FAILED_FB);
        STR(ERROR_SPACE_CLOUD_STORAGE_DISABLED_FB);
        STR(ERROR_HINT_ALREADY_SET_QCOM);
        STR(RESULT_MAX_ENUM);
#undef STR
    default:
        return "XR_RESULT_PARSING_ERROR";
    }
}
#ifndef NDEBUG
XrBool32 xrCallback(
    XrDebugUtilsMessageSeverityFlagsEXT severity,
    XrDebugUtilsMessageTypeFlagsEXT,
    const XrDebugUtilsMessengerCallbackDataEXT* callbackData,
    void*)
{
    if (severity & XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        logger::warning(callbackData->message, "", "", NOT_PRINT_LINE_NUMBER);
    }
    else if (severity & XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        logger::error(callbackData->message, "", "", NOT_PRINT_LINE_NUMBER);
    }

    return XR_FALSE;
}

VkBool32 vkCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    void*)
{
    if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        logger::warning(callbackData->pMessage, "", "", NOT_PRINT_LINE_NUMBER);
    }
    else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        auto throwException = true;


        if (gXrDeviceId == khronos_utils::device_ids::HtcVivePro)
        {
            throwException = false;
        }

        if (!throwException)
        {
            static std::optional<XrDeviceId> warnedXrDeviceId;

            if ((warnedXrDeviceId == std::nullopt) || warnedXrDeviceId != gXrDeviceId)
            {
                warnedXrDeviceId = std::make_optional<XrDeviceId>(gXrDeviceId);

                const auto it = std::ranges::find_if(knownXrDevicesIdToName, [](const auto& deviceInfo) -> bool {
                    return deviceInfo.first == warnedXrDeviceId;
                });

                if (it == knownXrDevicesIdToName.end())
                {
                    LOGGER_ERR("Your xr device is marked by the engine as the unstable, but can not find its name.");
                }

                LOGGER_WARN(("Unstable OpenXr device in use ("s + it->second.data() + "). " +
                    "Vulkan validation layers's errors won't stop the program.").c_str());
            }
        }

        logger::error(callbackData->pMessage, "", "", NOT_PRINT_LINE_NUMBER, throwException, throwException);
    }

    return VK_FALSE;
}
#endif // !NDEBUG

void unpackXrExtensionString(const std::string& str, std::vector<std::string>& result)
{
    std::istringstream stream(str);
    std::string extension;

    while (getline(stream, extension, ' '))
    {
        result.emplace_back(std::move(extension));
    }
}

XrPosef makeXrIdentity()
{
    const XrPosef identity2{
        .orientation = {0.0f, 0.0f, 0.0f, 1.0f},
        .position = {0.0f, 0.0f, 0.0f},
    };

    return identity2;
}

bool findSuitableMemoryTypeIndex(
    VkPhysicalDevice pPhysicalDevice,
    VkMemoryRequirements pRequirements,
    VkMemoryPropertyFlags pProperties,
    uint32_t& typeIndex)
{
    VkPhysicalDeviceMemoryProperties supportedMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(pPhysicalDevice, &supportedMemoryProperties);

    const VkMemoryPropertyFlags typeFilter{ pRequirements.memoryTypeBits };
    for (uint32_t memoryTypeIndex{}; memoryTypeIndex < supportedMemoryProperties.memoryTypeCount; ++memoryTypeIndex)
    {
        const VkMemoryPropertyFlags propertyFlags{ supportedMemoryProperties.memoryTypes[memoryTypeIndex].propertyFlags };
        if ((typeFilter & (1 << memoryTypeIndex)) && ((propertyFlags & pProperties) == pProperties))
        {
            typeIndex = memoryTypeIndex;
            return true;
        }
    }

    return false;
}

VkDeviceSize align(VkDeviceSize value, VkDeviceSize alignment)
{
    if (value == 0u)
    {
        return value;
    }

    return (value + alignment - 1u) & ~(alignment - 1u);
}

math::Mat4 createXrProjectionMatrix(const XrFovf fov, const float nearClip, const float farClip)
{
    const auto l = tan(fov.angleLeft);
    const auto r = tan(fov.angleRight);
    const auto d = tan(fov.angleDown);
    const auto u = tan(fov.angleUp);

    const auto w = r - l;
    const auto h = d - u;

    math::Mat4 projectionMatrix
    {
        2.f / w    , 0.f        , 0.f                                                      , 0.0f ,
        0.f        , 2.f / h    , 0.f                                                      , 0.0f ,
        (r + l) / w, (u + d) / h, -(farClip + nearClip) / (farClip - nearClip)             , -1.0f,
        0.f        , 0.f        , -(farClip * (nearClip + nearClip)) / (farClip - nearClip), 0.0f ,
    };

    return projectionMatrix;
}

math::Mat4 xrPoseToMatrix(const XrPosef& pose)
{
    const auto translation =
        math::translate(math::Mat4(1.f), { pose.position.x, pose.position.y, pose.position.z });

    const auto rotation =
        math::Mat4(math::Quat(pose.orientation.w, pose.orientation.x, pose.orientation.y, pose.orientation.z));

    return translation * rotation;
}
} // namespace ts::khronos_utils