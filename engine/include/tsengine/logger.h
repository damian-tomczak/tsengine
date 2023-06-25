#pragma once

#ifdef TSENGINE_BULDING
    #include "pch.h"
    #include "vulkan/vulkan_functions.h"
    #include "openxr/openxr_platform.h"
#endif

#ifdef WIN32
    #define FUNCTION_SIGNATURE __FUNCSIG__
#else
    #error "not implemented"
#endif

#define LOGGER_LOG(message) ts::logger::log(message, __FILE__, FUNCTION_SIGNATURE, __LINE__)
#define LOGGER_WARN(message) ts::logger::warning(message, __FILE__, FUNCTION_SIGNATURE, __LINE__)
#define LOGGER_ERR(message) ts::logger::error(message, __FILE__, FUNCTION_SIGNATURE, __LINE__)

#ifdef TSENGINE_BULDING
#define LOGGER_VK(function, ...)                                                                \
    {                                                                                           \
        VkResult result{function(__VA_ARGS__)};                                                 \
        if (result != VK_SUCCESS)                                                               \
        {                                                                                       \
            ts::logger::error(                                                                  \
                (#function " failed with status: " + logger::vkResultToString(result)).c_str(), \
                __FILE__,                                                                       \
                __FUNCTION__,                                                                   \
                __LINE__);                                                                      \
        }                                                                                       \
                                                                                                \
    }

#define LOGGER_XR(function, ...)                                                                \
    {                                                                                           \
        XrResult result{function(__VA_ARGS__)};                                                 \
        if (result != XR_SUCCESS)                                                               \
        {                                                                                       \
            ts::logger::error(                                                                  \
                (#function " failed with status: " + logger::xrResultToString(result)).c_str(), \
                __FILE__,                                                                       \
                __FUNCTION__,                                                                   \
                __LINE__);                                                                      \
        }                                                                                       \
                                                                                                \
    }
#endif

namespace ts
{
namespace logger
{
    void log(
        const char* message,
        const char* fileName,
        const char* functionName,
        int lineNumber);

    void warning(
        const char* message,
        const char* fileName,
        const char* functionName,
        int lineNumber);

    void error(
        const char* message,
        const char* fileName,
        const char* functionName,
        int lineNumber);

#ifdef TSENGINE_BULDING
    // TODO: vkResultToString doesn't cover every VkResult
    std::string vkResultToString(VkResult result);
    std::string xrResultToString(XrResult result);

#ifdef DEBUG
    constexpr XrDebugUtilsMessageSeverityFlagsEXT xrDebugUtilsMessageSeverityFlags =
        XR_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
        XR_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    constexpr XrDebugUtilsMessageTypeFlagsEXT xrDebugUtilsMessageTypeFlags =
        XR_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        XR_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | XR_DEBUG_UTILS_MESSAGE_TYPE_CONFORMANCE_BIT_EXT;

    XrBool32 xrCallback(XrDebugUtilsMessageSeverityFlagsEXT messageSeverity,
        XrDebugUtilsMessageTypeFlagsEXT messageTypes,
        const XrDebugUtilsMessengerCallbackDataEXT* callbackData,
        void* userData);
#endif // DEBUG
#endif // TSENGINE_BUILDING
} // namespace logger
} // namespace ts
