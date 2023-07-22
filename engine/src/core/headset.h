#pragma once

#include "context.h"
#include "tsengine/logger.h"

namespace ts
{
class Headset final
{
    NOT_COPYABLE_AND_MOVEABLE(Headset);

    static constexpr XrReferenceSpaceType spaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
    static constexpr VkFormat colorFormat = VK_FORMAT_R8G8B8A8_SRGB;
    static constexpr VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;

public:
    Headset(Context& ctx) : mCtx(ctx)
    {}
    ~Headset();

    void createRenderPass();
    void createXrSession();
    void createXrSpace();
    void createViews();

private:
    Context& mCtx;
    VkRenderPass mpVkRenderPass;
    XrSession mpXrSession{};
    XrSpace mpXrSpace{};
    uint32_t mEyeCount;
    std::vector<XrViewConfigurationView> mEyeViewInfos;
    std::vector<XrView> mEyePoses;
};
} // namespace ts
