#pragma once

#include "internal_utils.h"
#include "tsengine/math.hpp"

#include "openxr/openxr.h"
#include "vulkan/vulkan.h"

namespace ts
{
class Context;
class ImageBuffer;
class RenderTarget;

class Headset final
{
    TS_NOT_COPYABLE_AND_MOVEABLE(Headset);

    static constexpr XrReferenceSpaceType spaceType{XR_REFERENCE_SPACE_TYPE_STAGE};
    static constexpr VkFormat colorFormat{VK_FORMAT_R8G8B8A8_SRGB};
    static constexpr VkFormat depthFormat{VK_FORMAT_D32_SFLOAT};

public:
    Headset(const Context& ctx);
    ~Headset();

    enum class BeginFrameResult
    {
        RENDER_FULLY,
        RENDER_SKIP_PARTIALLY,
        RENDER_SKIP_FULLY
    };

    void init();

    BeginFrameResult beginFrame(uint32_t& swapchainImageIndex);

    void createVkRenderPass();
    void createXrSession();
    void createXrSpace();
    void createXrSwapchain();
    void endFrame(bool skipReleaseSwapchainImage) const;

    [[nodiscard]] XrSession getXrSession() const { return mXrSession; }
    [[nodiscard]] VkRenderPass getVkRenderPass() const { return mVkRenderPass; }
    [[nodiscard]] bool isExitRequested() const { return mIsExitRequested; }
    [[nodiscard]] XrSpace getXrSpace() const { return mXrSpace; }
    [[nodiscard]] XrFrameState getXrFrameState() const { return mXrFrameState; }
    [[nodiscard]] std::shared_ptr<RenderTarget> getRenderTarget(size_t swapchainImageIndex) const { return mSwapchainRenderTargets.at(swapchainImageIndex); }
    [[nodiscard]] VkExtent2D getEyeResolution(int32_t eyeIndex) const;
    [[nodiscard]] size_t getEyeCount() const { return mEyeCount; }
    [[nodiscard]] math::Mat4 getEyeViewMatrix(size_t eyeIndex) const { return mEyeviewMats.at(eyeIndex); }
    [[nodiscard]] math::Mat4 getEyeProjectionMatrix(size_t eyeIndex) const { return mEyeProjectionMatrices.at(eyeIndex); }

private:
    void createViews();
    void beginSession() const;
    void endSession() const;

    const Context& mCtx;
    VkRenderPass mVkRenderPass{};
    XrSession mXrSession{};
    XrSpace mXrSpace{};
    uint32_t mEyeCount{};
    std::vector<XrViewConfigurationView> mEyeViewInfos;
    std::vector<XrView> mEyePoses;
    std::unique_ptr<ImageBuffer> mColorBuffer;
    std::unique_ptr<ImageBuffer> mDepthBuffer;
    XrSwapchain mXrSwapchain{};
    std::vector<std::shared_ptr<RenderTarget>> mSwapchainRenderTargets;
    std::vector<XrCompositionLayerProjectionView> mEyeRenderInfos;
    std::vector<math::Mat4> mEyeviewMats;
    std::vector<math::Mat4> mEyeProjectionMatrices;
    bool mIsExitRequested{};
    XrFrameState mXrFrameState{};
    XrSessionState mXrSessionState{};
    XrViewState mXrViewState{};
};
} // namespace ts
