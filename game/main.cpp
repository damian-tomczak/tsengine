#include "game.h"

#include "tsengine/core.h"

int main()
{
    try
    {
        auto returnedValue{ ts::run(new Game) };
        if (returnedValue)
        {
            // TODO: logger
        }
    }
    catch (const std::exception& e)
    {
        puts(e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}