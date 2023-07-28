#pragma once

#include "tsengine/math.hpp"
#include "utils.hpp"
#include "openxr/openxr.h"
#include "vulkan/vulkan.h"

namespace ts
{
class Context;
class ImageBuffer;
class RenderTarget;

class Headset final
{
    NOT_COPYABLE_AND_MOVEABLE(Headset);

    static constexpr XrReferenceSpaceType spaceType{XR_REFERENCE_SPACE_TYPE_STAGE};
    static constexpr VkFormat colorFormat{VK_FORMAT_R8G8B8A8_SRGB};
    static constexpr VkFormat depthFormat{VK_FORMAT_D32_SFLOAT};

public:
    Headset(const Context* ctx);
    ~Headset();

    enum class BeginFrameResult
    {
        RENDER_FULLY,
        RENDER_SKIP_PARTIALLY,
        RENDER_SKIP_FULLY
    };

    BeginFrameResult beginFrame(uint32_t& swapchainImageIndex);

    void createRenderPass();
    void createXrSession();
    void createXrSpace();
    void createSwapchain();

    XrSession getXrSession() const { return mXrSession; }
    VkRenderPass getVkRenderPass() const { return mVkRenderPass; }
    bool isExitRequested() const { return mIsExitRequested; }
    XrSpace getXrSpace() const { return mXrSpace; }
    XrFrameState getXrFrameState() const { return mXrFrameState; }
    std::shared_ptr<RenderTarget> getRenderTarget(size_t swapchainImageIndex) const { return mSwapchainRenderTargets.at(swapchainImageIndex); }
    VkExtent2D getEyeResolution(int32_t eyeIndex) const
    {
        const XrViewConfigurationView& eyeInfo{mEyeViewInfos.at(eyeIndex)};
        return {eyeInfo.recommendedImageRectWidth, eyeInfo.recommendedImageRectHeight};
    }
    size_t getEyeCount() const { return mEyeCount; }
    math::Mat4 getEyeViewMatrix(size_t eyeIndex) const { return mEyeViewMatrices.at(eyeIndex); };
    math::Mat4 getEyeProjectionMatrix(size_t eyeIndex) const { return mEyeProjectionMatrices.at(eyeIndex); };

private:
    void createViews();
    void beginSession() const;
    void endSession() const;

    const Context* mCtx{};
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
    std::vector<math::Mat4> mEyeViewMatrices;
    std::vector<math::Mat4> mEyeProjectionMatrices;
    bool mIsExitRequested{};
    XrFrameState mXrFrameState{};
    XrSessionState mXrSessionState{};
    XrViewState mXrViewState{};
};
} // namespace ts
