#pragma once

#include "utils.hpp"

#include "vulkan/vulkan.h"

namespace ts
{
class Context;
class Window;

class MirrorView final
{
    NOT_COPYABLE_AND_MOVEABLE(MirrorView);

public:
    MirrorView(const Context* pCtx, std::shared_ptr<Window> pWindow);

    VkSurfaceKHR getSurface() const { return mSurface; }

private:
    void createSurface();

    const Context* mpCtx{};
    const std::weak_ptr<Window> mpWindow;
    VkSurfaceKHR mSurface{};
};
}