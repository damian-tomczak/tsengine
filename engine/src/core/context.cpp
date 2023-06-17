#include "context.h"

namespace ts
{
Context::Context()
{
    compileShaders();
}

void Context::connectWithVkLoaderLibrary()
{
#if defined _WIN32
    vkLibrary= LoadLibrary("vulkan-1.dll");
#endif

    if (vkLibrary == nullptr)
    {
        // TODO: logger
    }
}
} // namespace ts