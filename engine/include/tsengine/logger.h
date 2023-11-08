#pragma once

#ifdef _WIN32
#define FUNCTION_SIGNATURE __FUNCSIG__
#else
#error not implemented
#endif // _WIN32

#define NOT_PRINT_LINE_NUMBER -1

#define TS_LOG(msg) ts::logger::log(msg, __FILE__, FUNCTION_SIGNATURE, __LINE__)
#define TS_WARN(msg) ts::logger::warning(msg, __FILE__, FUNCTION_SIGNATURE, __LINE__)
#define TS_ERR(msg) ts::logger::error(msg, __FILE__, FUNCTION_SIGNATURE, __LINE__)

#ifndef NDEBUG
#define TS_ASSERT(condition)                                                                   \
    if (!(condition))                                                                          \
    {                                                                                          \
        ts::logger::warning("Assertion Failed", __FILE__, FUNCTION_SIGNATURE, __LINE__, true); \
    }

#define TS_ASSERT_MSG(condition, msg)                                           \
    if (!(condition))                                                           \
    {                                                                           \
        ts::logger::warning(msg, __FILE__, FUNCTION_SIGNATURE, __LINE__, true); \
    }

#else
#define TS_ASSERT(condition)
#define TS_ASSERT_MSG(condition, msg)
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
