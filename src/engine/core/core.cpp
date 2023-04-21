#include "tsengine/core.h"

#include <stdlib.h>

static unsigned tickCount{};
static auto isAlreadyInitiated{ false };

namespace ts
{
unsigned getTickCount()
{
    return tickCount;
}

int run(Engine* pEngine)
{
    if (pEngine == nullptr)
    {
        return EXIT_FAILURE;
    }

    if (isAlreadyInitiated)
    {
        return EXIT_FAILURE;
    }

    const char* pGameName;
    int width;
    int height;
    bool isFullscreen;
    pEngine->preInit(pGameName, width, height, isFullscreen);
    isAlreadyInitiated = true;

    // TODO: window setup

    pEngine->init();

    while (!pEngine->tick())
    {
        // TODO: mainloop
        if (false)
        {
            pEngine->onMouseMove(-1, -1, -1, -1);
        }

        if (false)
        {
            pEngine->onMouseButtonClick({}, false);
        }

        if (false)
        {
            pEngine->onKeyPressed({});
        }

        if (false)
        {
            pEngine->onKeyReleased({});
        }
    }

    pEngine->close();
    isAlreadyInitiated = false;

    return EXIT_SUCCESS;
}

} // namespace ts
