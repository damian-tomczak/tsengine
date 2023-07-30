#include "tsengine/logger.h"
#include "utils.hpp"

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

constexpr std::string_view colorToString(Color color)
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
        << "[INFO][" + currentDateTimeToString()
#ifndef NDEBUG
        << debugInfo(fileName, functionName, lineNumber)
#endif // NDEBUG
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
        << "[WARNING][" + currentDateTimeToString()
#ifndef NDEBUG
        << debugInfo(fileName, functionName, lineNumber)
#endif // NDEBUG
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
    bool throwException)
{
    std::lock_guard<std::mutex> _{loggerMutex};

    std::cerr
        << colorToString(Color::RED)
        << "[ERROR][" << currentDateTimeToString()
#ifndef NDEBUG
        << debugInfo(fileName, functionName, lineNumber)
#endif // NDEBUG
        << "]: "
        << formatingEnd
        << message
        << "\n";

#ifndef NDEBUG
#ifdef _WIN32
    DebugBreak();
#else
#error not implemented
#endif // _WIN32
#endif // DEBUG

    if (throwException)
    {
        throw TSException();
    }
}
} // namespace ts::logger