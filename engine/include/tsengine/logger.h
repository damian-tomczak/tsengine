#pragma once

#ifdef BUILDING_TSENGINE
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

#ifdef BUILDING_TSENGINE
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
        unsigned lineNumber);

    void warning(
        const char* message,
        const char* fileName,
        const char* functionName,
        unsigned lineNumber);

    void error(
        const char* message,
        const char* fileName,
        const char* functionName,
        unsigned lineNumber);

#ifdef BUILDING_TSENGINE
    // TODO: vkResultToString doesn't cover every VkResult
    std::string vkResultToString(VkResult result);
    std::string xrResultToString(XrResult result);
#endif
}
}
