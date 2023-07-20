#include "headset.h"

namespace
{
    constexpr XrReferenceSpaceType spaceType = XR_REFERENCE_SPACE_TYPE_STAGE;
    constexpr VkFormat colorFormat = VK_FORMAT_R8G8B8A8_SRGB;
    constexpr VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
} // namespace

namespace ts
{
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

    VkAttachmentDescription colorAttachmentDescription {
        .format = colorFormat,
        .samples = mCtx.getMultisampleCount(),
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
        .samples = mCtx.getMultisampleCount(),
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

void Headset::createSession()
{
    const auto mpVkInstance{mCtx.getVkInstance()};
    const auto mVkPhysicalDevice{mCtx.getVkPhysicalDevice()};
    const auto mVkLogicalDevice{mCtx.getVkDevice()};
    const auto mpVkGraphicsQueueFamilyIndex{mCtx.getGraphicsQueueFamilyIndex()};

    XrGraphicsBindingVulkanKHR graphicsBinding{
        .type = XR_TYPE_GRAPHICS_BINDING_VULKAN_KHR,
        .instance = mpVkInstance,
        .physicalDevice = mVkPhysicalDevice,
        .device = mVkLogicalDevice,
        .queueFamilyIndex = mpVkGraphicsQueueFamilyIndex,
        .queueIndex = 0u
    };

    const auto pXrSystemId{mCtx.getXrSystemId()};
    XrSessionCreateInfo sessionCreateInfo{
        .type = XR_TYPE_SESSION_CREATE_INFO,
        .next = &graphicsBinding,
        .systemId = pXrSystemId
    };

    LOGGER_XR(xrCreateSession, mCtx.getXrInstance(), &sessionCreateInfo, &mpXrSession);
}

Headset::~Headset()
{
    if (mpXrSession != nullptr)
    {
        xrDestroySession(mpXrSession);
    }

    const VkDevice vkDevice{mCtx.getVkDevice()};
    if (vkDevice && mpVkRenderPass)
    {
        vkDestroyRenderPass(vkDevice, mpVkRenderPass, nullptr);
    }
}
} // namespace ts
