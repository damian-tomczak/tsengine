#include "tsengine/logger.h"

namespace
{
    constexpr auto loggerSuffix{"\033[0m\n"};
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
    std::lock_guard<std::mutex> _{loggerMutex};

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
    std::lock_guard<std::mutex> _{loggerMutex};

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
    std::lock_guard<std::mutex> _{loggerMutex};

    std::cerr
        << colorToString(Color::RED)
        << "LOG [" << currentDateTimeToString()
#ifndef NDEBUG
        << debugInfo(fileName, functionName, lineNumber)
#endif // DEBUG
        << "]: "
        << message
        << loggerSuffix;

#ifndef NDEBUG
#ifdef _WIN32
    DebugBreak();
#else
#error not implemented
#endif // _WIN32
#endif // DEBUG

    if (isThrowingExc)
    {
        throw TSException();
    }
}
} // namespace ts::logger