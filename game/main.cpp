#include "game.h"

#include "tsengine/core.h"
#include "tsengine/logger.h"

int main()
{
    try
    {
        auto result{ ts::run(new Game) };
        if (result)
        {
            return result;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}