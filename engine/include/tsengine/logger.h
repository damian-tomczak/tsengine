#pragma once

#ifdef TSENGINE_BULDING
#include "vulkan/vulkan.h"
#include "openxr/openxr.h"

class TSException : public std::exception
{
public:
    TSException() {}
};

#endif // TSENGINE_BULDING

#ifdef _WIN32
#define FUNCTION_SIGNATURE __FUNCSIG__
#else
#error "not implemented"
#endif // _WIN32

#define NOT_PRINT_LINE_NUMBER -1

#define LOGGER_LOG(message) ts::logger::log(message, __FILE__, FUNCTION_SIGNATURE, __LINE__)
#define LOGGER_WARN(message) ts::logger::warning(message, __FILE__, FUNCTION_SIGNATURE, __LINE__)
#define LOGGER_ERR(message) ts::logger::error(message, __FILE__, FUNCTION_SIGNATURE, __LINE__)

namespace ts
{
    enum
    {
        SUCCESS,
        TS_FAILURE,
        STL_FAILURE,
        UNKNOWN_FAILURE
    };

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
        int lineNumber,
        bool isThrowingExc = true);
} // namespace logger
} // namespace ts
