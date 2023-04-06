#include "tsengine/core.h"

unsigned tickCount{};

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
        return 1;
    }
    return 0;
}

}
