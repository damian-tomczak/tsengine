#pragma once

#include "context.h"
#include "image_buffer.h"
#include "render_target.h"
#include "tsengine/math.hpp"

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
    void createSwapchain();

    XrSession getXrSession() const { return mXrSession; }

private:
    void createViews();
    VkExtent2D getEyeResolution(int32_t eyeIndex) const
    {
        const XrViewConfigurationView& eyeInfo = mEyeViewInfos.at(eyeIndex);
        return {eyeInfo.recommendedImageRectWidth, eyeInfo.recommendedImageRectHeight};
    }

    Context& mCtx;
    VkRenderPass mVkRenderPass{};
    XrSession mXrSession{};
    XrSpace mXrSpace{};
    uint32_t mEyeCount{};
    std::vector<XrViewConfigurationView> mEyeViewInfos;
    std::vector<XrView> mEyePoses;
    std::unique_ptr<ImageBuffer> mColorBuffer;
    std::unique_ptr<ImageBuffer> mDepthBuffer;
    XrSwapchain mXrSwapchain{};
    std::vector<std::unique_ptr<RenderTarget>> mSwapchainRenderTargets;
    std::vector<XrCompositionLayerProjectionView> mEyeRenderInfos;
    std::vector<Matrix4x4> mEyeViewMatrices;
    std::vector<Matrix4x4> mEyeProjectionMatrices;
};
} // namespace ts
