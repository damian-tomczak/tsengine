#include "headset.h"

namespace ts
{
Headset::~Headset()
{
    if (mpXrSession)
    {
        xrEndSession(mpXrSession);
    }

    if (mpXrSession != nullptr)
    {
        xrDestroySession(mpXrSession);
    }

    const VkDevice pVkDevice{ mCtx.getVkDevice() };
    if (pVkDevice && mpVkRenderPass)
    {
        vkDestroyRenderPass(pVkDevice, mpVkRenderPass, nullptr);
    }
}

void Headset::createRenderPass()
{
    constexpr uint32_t viewMask = 0b11;
    constexpr uint32_t correlationMask = 0b11;

    VkRenderPassMultiviewCreateInfo renderPassMultiviewCreateInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_MULTIVIEW_CREATE_INFO,
        .subpassCount = 1u,
        .pViewMasks = &viewMask,
        .correlationMaskCount = 1u,
        .pCorrelationMasks = &correlationMask
    };

    const auto multisampleCount{mCtx.getMultisampleCount()};
    VkAttachmentDescription colorAttachmentDescription {
        .format = colorFormat,
        .samples = multisampleCount,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkAttachmentReference colorAttachmentReference {
        .attachment = 0u,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    VkAttachmentDescription depthAttachmentDescription{
        .format = depthFormat,
        .samples = multisampleCount,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkAttachmentReference depthAttachmentReference {
        .attachment = 1u,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkAttachmentDescription resolveAttachmentDescription{
        .format = colorFormat,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkAttachmentReference resolveAttachmentReference{
        .attachment = 2u,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpassDescription {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1u,
        .pColorAttachments = &colorAttachmentReference,
        .pResolveAttachments = &resolveAttachmentReference,
        .pDepthStencilAttachment = &depthAttachmentReference,
    };

    const std::array attachments {
        colorAttachmentDescription,
        depthAttachmentDescription,
        resolveAttachmentDescription
    };

    VkRenderPassCreateInfo renderPassCreateInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = &renderPassMultiviewCreateInfo,
        .attachmentCount = static_cast<uint32_t>(attachments.size()),
        .pAttachments = attachments.data(),
        .subpassCount = 1u,
        .pSubpasses = &subpassDescription
    };

    const auto pVkDevice{mCtx.getVkDevice()};
    LOGGER_VK(vkCreateRenderPass, pVkDevice, &renderPassCreateInfo, nullptr, &mpVkRenderPass);
}

void Headset::createXrSession()
{
    const auto mpVkInstance{mCtx.getVkInstance()};
    const auto mpVkPhysicalDevice{mCtx.getVkPhysicalDevice()};
    const auto mpVkDevice{mCtx.getVkDevice()};
    const auto mVkGraphicsQueueFamilyIndex{mCtx.getGraphicsQueueFamilyIndex()};

    XrGraphicsBindingVulkanKHR graphicsBinding{
        .type = XR_TYPE_GRAPHICS_BINDING_VULKAN_KHR,
        .instance = mpVkInstance,
        .physicalDevice = mpVkPhysicalDevice,
        .device = mpVkDevice,
        .queueFamilyIndex = mVkGraphicsQueueFamilyIndex,
        .queueIndex = 0u
    };

    const auto pXrSystemId{mCtx.getXrSystemId()};
    XrSessionCreateInfo sessionCreateInfo{
        .type = XR_TYPE_SESSION_CREATE_INFO,
        .next = &graphicsBinding,
        .systemId = pXrSystemId
    };

    const auto pXrInstance{mCtx.getXrInstance()};
    LOGGER_XR(xrCreateSession, pXrInstance, &sessionCreateInfo, &mpXrSession);
}

void Headset::createXrSpace()
{
    const auto identity{ utils::makeIdentity() };
    XrReferenceSpaceCreateInfo referenceSpaceCreateInfo{
        .type = XR_TYPE_REFERENCE_SPACE_CREATE_INFO,
        .referenceSpaceType = spaceType,
        .poseInReferenceSpace = identity
    };

    LOGGER_XR(xrCreateReferenceSpace, mpXrSession, &referenceSpaceCreateInfo, &mpXrSpace);
}

void Headset::createViews()
{
    LOGGER_XR(xrEnumerateViewConfigurationViews,
        mCtx.getXrInstance(),
        mCtx.getXrSystemId(),
        mCtx.xrViewType,
        0u,
        reinterpret_cast<uint32_t*>(&mEyeCount), nullptr);

    mEyeViewInfos.resize(mEyeCount);
    for (auto& eyeInfo : mEyeViewInfos)
    {
        eyeInfo.type = XR_TYPE_VIEW_CONFIGURATION_VIEW;
        eyeInfo.next = nullptr;
    }

    LOGGER_XR(xrEnumerateViewConfigurationViews,
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
        eyePose.next = nullptr;
    }
}
} // namespace ts
