#pragma once

#include "utils.hpp"

#include "vulkan/vulkan.h"

namespace ts
{
class Context;
class Window;
class Headset;
class Renderer;

class MirrorView final
{
    NOT_COPYABLE_AND_MOVEABLE(MirrorView);

public:
    MirrorView(const Context* ctx, const std::shared_ptr<Window> window);

    void connect(const Headset* headset, const Renderer* renderer);

    VkSurfaceKHR getSurface() const { return mSurface; }

private:
    void createSurface();
    void recreateSwapchain();

    const Context* mCtx{};
    const std::weak_ptr<Window> mWindow;
    VkSurfaceKHR mSurface{};
    const Headset* mHeadset;
    const Renderer* mRenderer;
};
}