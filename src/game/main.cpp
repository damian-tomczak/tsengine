#include "game/game.h"

#include "tsengine/core.h"

#include <stdexcept>

int main(int argc, char** argv)
{
    try
    {
        auto returnValue{ ts::run(new Game) };
        if (returnValue)
        {
            throw std::runtime_error{ "TSEngine::run returned error: " + std::to_string(returnValue) };
        }
    }
    catch (const std::exception& e)
    {
        puts(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}