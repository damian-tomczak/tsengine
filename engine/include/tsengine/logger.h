#pragma once

#ifdef _WIN32
#define FUNCTION_SIGNATURE __FUNCSIG__
#else
#error not implemented
#endif // _WIN32

#define NOT_PRINT_LINE_NUMBER -1

#define TS_LOG(message) ts::logger::log(message, __FILE__, FUNCTION_SIGNATURE, __LINE__)
#define TS_WARN(message) ts::logger::warning(message, __FILE__, FUNCTION_SIGNATURE, __LINE__)
#define TS_ERR(message) ts::logger::error(message, __FILE__, FUNCTION_SIGNATURE, __LINE__)

// TODO: default message
#ifndef NDEBUG
#define TS_ASSERT(condition, ...)                                                       \
    if (!(condition))                                                                   \
    {                                                                                   \
        ts::logger::warning(__VA_ARGS__, __FILE__, FUNCTION_SIGNATURE, __LINE__, true); \
    }

#else
#define TS_ASSERT(condition, ...)
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
    int lineNumber,
    bool debugBreak = false);

void error(
    const char* message,
    const char* fileName,
    const char* functionName,
    int lineNumber,
    bool throwException = true,
    bool debugBreak = true);
} // namespace logger
} // namespace ts
