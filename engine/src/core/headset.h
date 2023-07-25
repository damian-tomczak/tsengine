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
    Headset(const Context* pCtx);
    ~Headset();

    void createRenderPass();
    void createXrSession();
    void createXrSpace();
    void createSwapchain();

    XrSession getXrSession() const { return mXrSession; }

private:
    void createViews();
    VkExtent2D getEyeResolution(int32_t eyeIndex) const;

    const Context* mpCtx;
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
    std::vector<math::Matrix4x4<>> mEyeViewMatrices;
    std::vector<math::Matrix4x4<>> mEyeProjectionMatrices;
};
} // namespace ts
