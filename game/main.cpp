#include "game.h"

#include "tsengine/core.h"

#include <stdexcept>

int main()
{
    try
    {
        auto returnedValue{ ts::run(new Game) };
        if (returnedValue)
        {
            throw std::runtime_error{ "TSEngine::run returned error: " + std::to_string(returnedValue) };
        }
    }
    catch (const std::exception& e)
    {
        puts(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}