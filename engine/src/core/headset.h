#pragma once

#include "context.h"
#include "tsengine/logger.h"

namespace ts
{
class Headset final
{
    NOT_COPYABLE_AND_MOVEABLE(Headset);

public:
    Headset(Context& ctx) : mCtx(ctx)
    {}
    ~Headset();

    void createRenderPass();
    void createSession();

private:
    Context& mCtx;
    VkRenderPass mpVkRenderPass;
    XrSession mpXrSession{};
};
} // namespace ts
