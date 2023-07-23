#pragma once

#include "context.h"
#include "os.h"

#include <memory>

namespace ts
{
class MirrorView final
{
    NOT_COPYABLE_AND_MOVEABLE(MirrorView);

public:
    MirrorView(Context& pContext, const std::unique_ptr<Window>& pWindow);

    VkSurfaceKHR getSurface() const { return mSurface; }

private:
    void createSurface();

    Context& mContext;
    const std::unique_ptr<Window>& mWindow;
    VkSurfaceKHR mSurface{};
};
}