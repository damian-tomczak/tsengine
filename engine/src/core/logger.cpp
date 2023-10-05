#include "tsengine/logger.h"
#include "internal_utils.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace
{
constexpr std::string_view formatingEnd{"\033[0m"};
std::mutex loggerMutex;

enum class Color
{
    GREEN,
    YELLOW,
    RED
};

consteval std::string_view colorToString(Color color)
{
    switch (color)
    {
    case Color::GREEN:
        return "\033[32m";
    case Color::YELLOW:
        return "\033[33m";
    case Color::RED:
        return "\033[91m";
    default:
        return "COLOR_PARSING_UNKNOWN";
    }
}

inline std::string currentDateTimeToStr()
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
#endif // !NDEBUG
} // namespace

namespace ts::logger
{
void log(
    const char* message,
    const char* fileName,
    const char* functionName,
    int lineNumber)
{
    std::lock_guard<std::mutex> _{loggerMutex};

    std::cout
        << colorToString(Color::GREEN)
        << "[INFO][" + currentDateTimeToStr()
#ifndef NDEBUG
        << debugInfo(fileName, functionName, lineNumber)
#endif // !NDEBUG
        + "]: "
        << formatingEnd
        << message
        << "\n";
}

void warning(
    const char* message,
    const char* fileName,
    const char* functionName,
    int lineNumber)
{
    std::lock_guard<std::mutex> _{loggerMutex};

    std::cout
        << colorToString(Color::YELLOW)
        << "[WARNING][" + currentDateTimeToStr()
#ifndef NDEBUG
        << debugInfo(fileName, functionName, lineNumber)
#endif // !NDEBUG
        + "]: "
        << formatingEnd
        << message
        << "\n";
}

void error(
    const char* message,
    const char* fileName,
    const char* functionName,
    int lineNumber,
    bool throwException,
    bool debugBreak)
{
    std::lock_guard<std::mutex> _{loggerMutex};

    std::cerr
        << colorToString(Color::RED)
        << "[ERROR][" << currentDateTimeToStr()
#ifndef NDEBUG
        << debugInfo(fileName, functionName, lineNumber)
#endif // !NDEBUG
        << "]: "
        << formatingEnd
        << message
        << "\n";

#ifndef NDEBUG
if (debugBreak)
{
#ifdef _WIN32
    DebugBreak();
#else
#error not implemented
#endif // _WIN32
}
#endif // !NDEBUG

    if (throwException)
    {
        throw TSException();
    }
}
} // namespace ts::logger