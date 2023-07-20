#include "tsengine/logger.h"

namespace
{
    constexpr auto loggerSuffix{ "\033[0m\n" };
    std::mutex loggerMutex;

    enum class Color
    {
        GREEN,
        YELLOW,
        RED
    };

    constexpr std::string_view colorToString(Color color)
    {
        switch (color)
        {
        case Color::GREEN:
            return "\x1B[32m";
        case Color::YELLOW:
            return "\x1B[33m";
        case Color::RED:
            return "\x1B[91m";
        default:
            return "COLOR_PARSING_UNKNOWN";
        }
    }

    std::string currentDateTimeToString()
    {
        return std::format("{:%d-%m-%Y %H:%M:%OS}",
            std::chrono::zoned_time(std::chrono::current_zone(), std::chrono::system_clock::now()));
    }

#ifndef NDEBUG
    inline auto debugInfo(std::string fileName, std::string functionName, int lineNumber)
    {
        std::ostringstream ss;
        if (fileName.length() || functionName.length())
        {
            ss << " at";
        }

        if (fileName.length())
        {
            ss << " " << fileName;
        }

        if (functionName.length())
        {
            ss << " " << functionName;
        }

        if (lineNumber != NOT_PRINT_LINE_NUMBER)
        {
            ss << ":" + std::to_string(lineNumber);
        }

        return ss.str();
    }
#endif // DEBUG
}

namespace ts::logger
{
void log(
    const char* message,
    const char* fileName,
    const char* functionName,
    int lineNumber)
{
    std::lock_guard<std::mutex> lock(loggerMutex);

    std::cout
        << colorToString(Color::GREEN)
        << "LOG [" + currentDateTimeToString()
#ifndef NDEBUG
        << debugInfo(fileName, functionName, lineNumber)
#endif // DEBUG
        + "]: "
        << message
        << loggerSuffix;
}

void warning(
    const char* message,
    const char* fileName,
    const char* functionName,
    int lineNumber)
{
    std::lock_guard<std::mutex> lock(loggerMutex);

    std::cout
        << colorToString(Color::YELLOW)
        << "WARN [" + currentDateTimeToString()
#ifndef NDEBUG
        << debugInfo(fileName, functionName, lineNumber)
#endif // DEBUG
        + "]: "
        << message
        << loggerSuffix;
}

void error(
    const char* message,
    const char* fileName,
    const char* functionName,
    int lineNumber,
    bool isThrowingExc)
{
    std::lock_guard<std::mutex> lock(loggerMutex);

    std::cerr <<
        colorToString(Color::RED)
        << "LOG [" << currentDateTimeToString()
#ifndef NDEBUG
        << debugInfo(fileName, functionName, lineNumber)
#endif // DEBUG
        << "]: "
        << message
        << loggerSuffix;

#ifndef NDEBUG
#ifdef _WIN32
    //DebugBreak();
#else
#error not implemented
#endif // _WIN32
#endif // DEBUG

    if (isThrowingExc)
    {
        throw TSException();
    }
}

#ifdef TSENGINE_BULDING
std::string vkResultToString(VkResult result)
{
    switch (result)
    {
#define STR(r) case VK_##r: return #r
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
        STR(ERROR_SURFACE_LOST_KHR);
        STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
        STR(SUBOPTIMAL_KHR);
        STR(ERROR_OUT_OF_DATE_KHR);
        STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
        STR(ERROR_VALIDATION_FAILED_EXT);
        STR(ERROR_INVALID_SHADER_NV);
#undef STR
    default:
        return "VK_RESULT_PARSING_UNKNOWN";
    }
}

std::string xrResultToString(XrResult result)
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
        if (severity >= XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            warning(callbackData->message, "", "", NOT_PRINT_LINE_NUMBER);
        }

        return XR_FALSE;
    }

    VkBool32 vkCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT,
        const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
        void*)
    {
        if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        {
            warning(callbackData->pMessage, "", "", NOT_PRINT_LINE_NUMBER);
        }

        return VK_FALSE;
    }
#endif // DEBUG
#endif // TSENGINE_BUILDING
} // namespace ts::logger