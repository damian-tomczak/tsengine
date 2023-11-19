#include "headset.h"
#include "context.h"
#include "image_buffer.h"
#include "render_target.h"
#include "openxr/openxr_platform.h"
#include "khronos_utils.h"
#include "renderer.h"
#include "vulkan_tools/vulkan_functions.h"

namespace ts
{
inline namespace TS_VER
{
Headset::Headset(const Context& ctx) : mCtx(ctx)
{}

Headset::~Headset()
{
    if (mXrSession != nullptr)
    {
        xrEndSession(mXrSession);
    }

    if (mXrSwapchain != nullptr)
    {
        xrDestroySwapchain(mXrSwapchain);
    }

    if (mXrSpace != nullptr)
    {
        xrDestroySpace(mXrSpace);
    }

    if (mXrSession != nullptr)
    {
        xrDestroySession(mXrSession);
    }

    const auto device = mCtx.getVkDevice();
    if (device != nullptr && mVkRenderPass != nullptr)
    {
        vkDestroyRenderPass(device, mVkRenderPass, nullptr);
    }
}

void Headset::init()
{
    createVkRenderPass();
    createXrSession();
    createXrSpace();
    createXrSwapchain();
}

Headset::BeginFrameResult Headset::beginFrame(uint32_t& swapchainImageIndex)
{
    XrEventDataBuffer buffer{XR_TYPE_EVENT_DATA_BUFFER};
    while (xrPollEvent(mCtx.getXrInstance(), &buffer) == XR_SUCCESS)
    {
        switch (buffer.type)
        {
        case XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING:
        {
            mIsExitRequested = true;
            return BeginFrameResult::RENDER_SKIP_FULLY;
        }
        case XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED:
        {
            const auto event = reinterpret_cast<XrEventDataSessionStateChanged*>(&buffer);
            mXrSessionState = event->state;

            if (event->state == XR_SESSION_STATE_READY)
            {
                beginSession();
            }
            else if (event->state == XR_SESSION_STATE_STOPPING)
            {
                endSession();
            }
            else if ((event->state == XR_SESSION_STATE_LOSS_PENDING) || (event->state == XR_SESSION_STATE_EXITING))
            {
                mIsExitRequested = true;
                return BeginFrameResult::RENDER_SKIP_FULLY;
            }

            break;
        }
        }
    }

    if ((mXrSessionState != XR_SESSION_STATE_READY) &&
        (mXrSessionState != XR_SESSION_STATE_SYNCHRONIZED) &&
        (mXrSessionState != XR_SESSION_STATE_VISIBLE) &&
        (mXrSessionState != XR_SESSION_STATE_FOCUSED))
    {
        return BeginFrameResult::RENDER_SKIP_FULLY;
    }

    mXrFrameState.type = XR_TYPE_FRAME_STATE;
    XrFrameWaitInfo frameWaitInfo{XR_TYPE_FRAME_WAIT_INFO};
    TS_XR_CHECK(xrWaitFrame, mXrSession, &frameWaitInfo, &mXrFrameState);

    XrFrameBeginInfo frameBeginInfo{XR_TYPE_FRAME_BEGIN_INFO};
    TS_XR_CHECK(xrBeginFrame, mXrSession, &frameBeginInfo);

    if (!mXrFrameState.shouldRender)
    {
        return BeginFrameResult::RENDER_SKIP_PARTIALLY;
    }

    mXrViewState.type = XR_TYPE_VIEW_STATE;
    XrViewLocateInfo viewLocateInfo{
        .type = XR_TYPE_VIEW_LOCATE_INFO,
        .viewConfigurationType = mCtx.xrViewType,
        .displayTime = mXrFrameState.predictedDisplayTime,
        .space = mXrSpace
    };
    uint32_t viewCount;
    TS_XR_CHECK(xrLocateViews,
        mXrSession,
        &viewLocateInfo,
        &mXrViewState,
        static_cast<uint32_t>(mEyePoses.size()),
        &viewCount,
        mEyePoses.data());

    if (viewCount != mEyeCount)
    {
        TS_ERR("Trying to display more views than defined eyes");
    }

    for (size_t eyeIndex{}; eyeIndex < mEyeCount; ++eyeIndex)
    {
        auto& eyeRenderInfo = mEyeRenderInfos.at(eyeIndex);
        const auto& eyePose = mEyePoses.at(eyeIndex);
        eyeRenderInfo.pose = eyePose.pose;
        eyeRenderInfo.fov = eyePose.fov;

        const auto& pose = eyeRenderInfo.pose;
        mEyeviewMats.at(eyeIndex) = math::inverse(khronos_utils::xrPoseToMatrix(pose));
        mEyeProjectionMatrices.at(eyeIndex) = khronos_utils::createXrProjectionMatrix(eyeRenderInfo.fov, 0.01f, 250.0f);
    }

    XrSwapchainImageAcquireInfo swapchainImageAcquireInfo{ XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO };
    TS_XR_CHECK(xrAcquireSwapchainImage, mXrSwapchain, &swapchainImageAcquireInfo, &swapchainImageIndex);

    XrSwapchainImageWaitInfo swapchainImageWaitInfo{
        .type = XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO,
        .timeout = XR_INFINITE_DURATION,
    };
    TS_XR_CHECK(xrWaitSwapchainImage, mXrSwapchain, &swapchainImageWaitInfo);

    return BeginFrameResult::RENDER_FULLY;
}

void Headset::createVkRenderPass()
{
    constexpr uint32_t viewMask{0b11};
    constexpr uint32_t correlationMask{0b11};

    const VkRenderPassMultiviewCreateInfo renderPassMultiviewCreateInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO,
        .subpassCount = 1,
        .pViewMasks = &viewMask,
        .correlationMaskCount = 1,
        .pCorrelationMasks = &correlationMask
    };

    const auto multisampleCount = mCtx.getVkMultisampleCount();
    const VkAttachmentDescription colorAttachmentDescription{
        .format = colorFormat,
        .samples = multisampleCount,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    const VkAttachmentReference colorAttachmentReference{
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    const VkAttachmentDescription depthAttachmentDescription{
        .format = depthFormat,
        .samples = multisampleCount,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    const VkAttachmentReference depthAttachmentReference{
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    const VkAttachmentDescription resolveAttachmentDescription{
        .format = colorFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    const VkAttachmentReference resolveAttachmentReference{
        .attachment = 2,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    const VkSubpassDescription subpassDescription{
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentReference,
        .pResolveAttachments = &resolveAttachmentReference,
        .pDepthStencilAttachment = &depthAttachmentReference,
    };

    const std::array attachments{
        colorAttachmentDescription,
        depthAttachmentDescription,
        resolveAttachmentDescription
    };

    const VkRenderPassCreateInfo renderPassCreateInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = &renderPassMultiviewCreateInfo,
        .attachmentCount = static_cast<uint32_t>(attachments.size()),
        .pAttachments = attachments.data(),
        .subpassCount = 1,
        .pSubpasses = &subpassDescription
    };

    const auto vkDevice = mCtx.getVkDevice();
    TS_VK_CHECK(vkCreateRenderPass, vkDevice, &renderPassCreateInfo, nullptr, &mVkRenderPass);
}

void Headset::createXrSession()
{
    const XrGraphicsBindingVulkanKHR graphicsBinding{
        .type = XR_TYPE_GRAPHICS_BINDING_VULKAN_KHR,
        .instance = mCtx.getVkInstance(),
        .physicalDevice = mCtx.getVkPhysicalDevice(),
        .device = mCtx.getVkDevice(),
        .queueFamilyIndex = mCtx.getVkGraphicsQueueFamilyIndex(),
        .queueIndex = 0
    };

    const XrSessionCreateInfo sessionCreateInfo{
        .type = XR_TYPE_SESSION_CREATE_INFO,
        .next = &graphicsBinding,
        .systemId = mCtx.getXrSystemId()
    };

    const auto pXrInstance{mCtx.getXrInstance()};
    TS_XR_CHECK(xrCreateSession, pXrInstance, &sessionCreateInfo, &mXrSession);
}

void Headset::createXrSpace()
{
    const auto identity{khronos_utils::makeXrIdentity()};
    XrReferenceSpaceCreateInfo referenceSpaceCreateInfo{
        .type = XR_TYPE_REFERENCE_SPACE_CREATE_INFO,
        .referenceSpaceType = spaceType,
        .poseInReferenceSpace = identity
    };

    TS_XR_CHECK(xrCreateReferenceSpace, mXrSession, &referenceSpaceCreateInfo, &mXrSpace);
}

void Headset::createViews()
{
    TS_XR_CHECK(xrEnumerateViewConfigurationViews,
        mCtx.getXrInstance(),
        mCtx.getXrSystemId(),
        mCtx.xrViewType,
        0,
        reinterpret_cast<uint32_t*>(&mEyeCount), nullptr);

    mEyeViewInfos.resize(mEyeCount);
    for (auto& eyeInfo : mEyeViewInfos)
    {
        eyeInfo.type = XR_TYPE_VIEW_CONFIGURATION_VIEW;
        eyeInfo.next = nullptr;
    }

    TS_XR_CHECK(xrEnumerateViewConfigurationViews,
        mCtx.getXrInstance(),
        mCtx.getXrSystemId(),
        mCtx.xrViewType,
        static_cast<uint32_t>(mEyeViewInfos.size()),
        reinterpret_cast<uint32_t*>(&mEyeCount),
        mEyeViewInfos.data());

    mEyePoses.resize(mEyeCount);
    for (auto& eyePose : mEyePoses)
    {
        eyePose.type = XR_TYPE_VIEW;
    }
}

void Headset::createXrSwapchain()
{
    createViews();

    uint32_t formatCount{};
    TS_XR_CHECK(xrEnumerateSwapchainFormats, mXrSession, 0, &formatCount, nullptr);

    std::vector<int64_t> formats(formatCount);
    TS_XR_CHECK(xrEnumerateSwapchainFormats, mXrSession, formatCount, &formatCount, formats.data());

    bool isFormatFound{};
    for (const auto& format : formats)
    {
        if (format == static_cast<int64_t>(colorFormat))
        {
            isFormatFound = true;
            break;
        }
    }

    if (!isFormatFound)
    {
        TS_ERR("openxr color doesn't support color format");
    }

    const auto eyeResolution = getEyeResolution(0);

    mColorBuffer = std::make_unique<ImageBuffer>(mCtx);
    mColorBuffer->createImage(
        eyeResolution,
        colorFormat,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        mCtx.getVkMultisampleCount(),
        VK_IMAGE_ASPECT_COLOR_BIT,
        2);

    mDepthBuffer = std::make_unique<ImageBuffer>(mCtx);
    mDepthBuffer->createImage(
        eyeResolution,
        depthFormat,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
        mCtx.getVkMultisampleCount(),
        VK_IMAGE_ASPECT_DEPTH_BIT,
        2);

    const XrViewConfigurationView& eyeImageInfo{mEyeViewInfos.at(0)};

    const XrSwapchainCreateInfo swapchainCreateInfo{
        .type = XR_TYPE_SWAPCHAIN_CREATE_INFO,
        .format = colorFormat,
        .sampleCount = eyeImageInfo.recommendedSwapchainSampleCount,
        .width = eyeImageInfo.recommendedImageRectWidth,
        .height = eyeImageInfo.recommendedImageRectHeight,
        .faceCount = 1,
        .arraySize = static_cast<uint32_t>(mEyeCount),
        .mipCount = 1
    };

    TS_XR_CHECK(xrCreateSwapchain, mXrSession, &swapchainCreateInfo, &mXrSwapchain);

    uint32_t swapchainImageCount;
    TS_XR_CHECK(xrEnumerateSwapchainImages, mXrSwapchain, 0, &swapchainImageCount, nullptr);

    std::vector<XrSwapchainImageVulkanKHR> swapchainImages;
    swapchainImages.resize(swapchainImageCount);
    for (auto& swapchainImage : swapchainImages)
    {
        swapchainImage.type = XR_TYPE_SWAPCHAIN_IMAGE_VULKAN_KHR;
    }

    auto pXrSwapchainImageBaseHeader = reinterpret_cast<XrSwapchainImageBaseHeader*>(swapchainImages.data());
    TS_XR_CHECK(xrEnumerateSwapchainImages,
        mXrSwapchain,
        static_cast<uint32_t>(swapchainImages.size()),
        &swapchainImageCount,
        pXrSwapchainImageBaseHeader);

    mSwapchainRenderTargets.resize(swapchainImages.size());
    for (size_t renderTargetIndex{}; renderTargetIndex < mSwapchainRenderTargets.size(); ++renderTargetIndex)
    {
        auto& pRenderTarget = mSwapchainRenderTargets.at(renderTargetIndex);

        const auto vkSwapchainImage = swapchainImages.at(renderTargetIndex).image;
        pRenderTarget = std::make_shared<RenderTarget>(mCtx.getVkDevice(), vkSwapchainImage);
        pRenderTarget->createRenderTarget(
            mColorBuffer->getVkImageView(),
            mDepthBuffer->getVkImageView(),
            eyeResolution,
            colorFormat,
            mVkRenderPass,
            2);
    }

    mEyeRenderInfos.resize(mEyeCount);
    for (size_t eyeIndex{}; eyeIndex < mEyeRenderInfos.size(); ++eyeIndex)
    {
        const XrViewConfigurationView& eyeImageInfo{mEyeViewInfos.at(eyeIndex)};

        XrCompositionLayerProjectionView& eyeRenderInfo = mEyeRenderInfos.at(eyeIndex);
        eyeRenderInfo.type = XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW;
        eyeRenderInfo.subImage.swapchain = mXrSwapchain;
        eyeRenderInfo.subImage.imageArrayIndex = static_cast<uint32_t>(eyeIndex);
        eyeRenderInfo.subImage.imageRect.offset = {0, 0};
        eyeRenderInfo.subImage.imageRect.extent = {
            static_cast<int32_t>(eyeImageInfo.recommendedImageRectWidth),
            static_cast<int32_t>(eyeImageInfo.recommendedImageRectHeight)
        };
    }

    mEyeviewMats.resize(mEyeCount);
    mEyeProjectionMatrices.resize(mEyeCount);
}

void Headset::endFrame(bool skipReleaseSwapchainImage) const
{
    if (!skipReleaseSwapchainImage)
    {
        XrSwapchainImageReleaseInfo swapchainImageReleaseInfo{ XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO };
        TS_XR_CHECK(xrReleaseSwapchainImage, mXrSwapchain, &swapchainImageReleaseInfo);
    }

    XrCompositionLayerProjection compositionLayerProjection{
        .type = XR_TYPE_COMPOSITION_LAYER_PROJECTION,
        .space = mXrSpace,
        .viewCount = static_cast<uint32_t>(mEyeRenderInfos.size()),
        .views = mEyeRenderInfos.data()
    };

    std::vector<XrCompositionLayerBaseHeader*> layers;

    const auto positionValid = mXrViewState.viewStateFlags & XR_VIEW_STATE_POSITION_VALID_BIT;
    const auto orientationValid = mXrViewState.viewStateFlags & XR_VIEW_STATE_ORIENTATION_VALID_BIT;
    if (mXrFrameState.shouldRender && positionValid && orientationValid)
    {
        layers.push_back(reinterpret_cast<XrCompositionLayerBaseHeader*>(&compositionLayerProjection));
    }

    XrFrameEndInfo frameEndInfo{
        .type = XR_TYPE_FRAME_END_INFO,
        .displayTime = mXrFrameState.predictedDisplayTime,
        .environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE,
        .layerCount = static_cast<uint32_t>(layers.size()),
        .layers = layers.data(),
    };
    TS_XR_CHECK(xrEndFrame, mXrSession, &frameEndInfo);
}

[[nodiscard]] VkExtent2D Headset::getEyeResolution(int32_t eyeIndex) const
{
    const XrViewConfigurationView& eyeInfo{mEyeViewInfos.at(eyeIndex)};
    return {eyeInfo.recommendedImageRectWidth, eyeInfo.recommendedImageRectHeight};
}

void Headset::beginSession() const
{
    XrSessionBeginInfo sessionBeginInfo{
        .type = XR_TYPE_SESSION_BEGIN_INFO,
        .primaryViewConfigurationType = mCtx.xrViewType
    };
    TS_XR_CHECK(xrBeginSession, mXrSession, &sessionBeginInfo);
}

void Headset::endSession() const
{
    TS_XR_CHECK(xrEndSession, mXrSession);
}
} // namespace ver
} // namespace ts
