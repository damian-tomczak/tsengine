#include "context.h"
#include "khronos_utils.h"
#include "openxr/openxr_platform.h"
#include "vulkan_tools/vulkan_loader.h"
#include "globals.hpp"

namespace ts
{
inline namespace TS_VER
{
namespace
{
    PFN_xrGetVulkanInstanceExtensionsKHR xrGetVulkanInstanceExtensionsKHR{};
    PFN_xrGetVulkanGraphicsDeviceKHR xrGetVulkanGraphicsDeviceKHR{};
    PFN_xrGetVulkanDeviceExtensionsKHR xrGetVulkanDeviceExtensionsKHR{};
    PFN_xrGetVulkanGraphicsRequirementsKHR xrGetVulkanGraphicsRequirementsKHR{};

#ifndef NDEBUG
    PFN_xrCreateDebugUtilsMessengerEXT xrCreateDebugUtilsMessengerEXT{};
    PFN_xrDestroyDebugUtilsMessengerEXT xrDestroyDebugUtilsMessengerEXT{};
#endif // !NDEBUG
} // namespace

Context& Context::createOpenXrContext()
{
    createXrInstance();
    loadXrExtensions();
#ifndef NDEBUG
    createXrDebugMessenger();
#endif
    initXrSystemId();
    initXrSystemInfo();
    isXrBlendModeAvailable();

    mIsXrContextCreated = true;

    return *this;
}

void Context::createVulkanContext()
{
    TS_ASSERT_MSG(mIsXrContextCreated, "XrContext should be firstly created");

    vkLoader::connectWithLoader();
    vkLoader::loadExportFunction();
    vkLoader::loadGlobalLevelFunctions();

    std::vector<std::string> vulkanInstanceExtensions;
    getRequiredVulkanInstanceExtensions(vulkanInstanceExtensions);
    isVulkanInstanceExtensionsAvailable(vulkanInstanceExtensions);

    createVulkanInstance(vulkanInstanceExtensions);

    vkLoader::loadInstanceLevelFunctions(mVkInstance, vulkanInstanceExtensions);

#ifdef _DEBUG
    vkLoader::loadDebugLevelFunctions(mVkInstance);
    createVkDebugMessenger();
#endif
}

void Context::createVkDevice(const VkSurfaceKHR vkMirrorSurface)
{
    createPhysicalDevice();
    getGraphicsQueue();
    getPresentQueue(vkMirrorSurface);

    std::vector<std::string> requiredVulkanDeviceExtensions;
    getRequiredVulkanDeviceExtensions(requiredVulkanDeviceExtensions);

    VkPhysicalDeviceFeatures physicalDeviceFeatures;
    VkPhysicalDeviceMultiviewFeatures physicalDeviceMultiviewFeatures{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES};
    VkPhysicalDeviceFeatures2 physicalDeviceFeatures2{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &physicalDeviceMultiviewFeatures
    };

    isVulkanDeviceExtensionsAvailable(
        requiredVulkanDeviceExtensions,
        physicalDeviceFeatures,
        physicalDeviceMultiviewFeatures,
        physicalDeviceFeatures2);

    std::vector<VkDeviceQueueCreateInfo> deviceQueueCis;
    createQueues(deviceQueueCis);

    createLogicalDevice(
        requiredVulkanDeviceExtensions,
        physicalDeviceFeatures,
        physicalDeviceMultiviewFeatures,
        deviceQueueCis);

    vkLoader::loadDeviceLevelFunctions(mVkDevice, requiredVulkanDeviceExtensions);

    vkGetDeviceQueue(mVkDevice, *mVkGraphicsQueueFamilyIndex, 0, &mVkGraphicsQueue);
    vkGetDeviceQueue(mVkDevice, *mVkPresentQueueFamilyIndex, 0, &mVkPresentQueue);
}

void Context::sync() const
{
    vkDeviceWaitIdle(mVkDevice);
}

#ifndef NDEBUG
void Context::createXrDebugMessenger()
{
    const XrDebugUtilsMessengerCreateInfoEXT ci{
        .type = XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverities = khronos_utils::xrDebugMessageSeverityFlags,
        .messageTypes = khronos_utils::xrDebugMessageTypeFlags,
        .userCallback = reinterpret_cast<PFN_xrDebugUtilsMessengerCallbackEXT>(khronos_utils::xrCallback)
    };

    TS_XR_CHECK(xrCreateDebugUtilsMessengerEXT,
        mXrInstance,
        &ci,
        &mXrDebugMessenger);
}

void Context::createVkDebugMessenger()
{
    const VkDebugUtilsMessengerCreateInfoEXT ci{
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = khronos_utils::vkDebugMessageSeverityFlags,
        .messageType = khronos_utils::vkDebugMessageTypeFlags,
        .pfnUserCallback = &khronos_utils::vkCallback
    };

    TS_VK_CHECK(vkCreateDebugUtilsMessengerEXT, mVkInstance, &ci, nullptr, &mVkDebugMessenger);
}
#endif // DEBUG

void Context::loadXrExtensions()
{
    TS_XR_CHECK(xrGetInstanceProcAddr,
        mXrInstance,
        "xrGetVulkanInstanceExtensionsKHR",
        reinterpret_cast<PFN_xrVoidFunction*>(&xrGetVulkanInstanceExtensionsKHR));

    TS_XR_CHECK(xrGetInstanceProcAddr,
        mXrInstance,
        "xrGetVulkanGraphicsDeviceKHR",
        reinterpret_cast<PFN_xrVoidFunction*>(&xrGetVulkanGraphicsDeviceKHR));

    TS_XR_CHECK(xrGetInstanceProcAddr,
        mXrInstance,
        "xrGetVulkanDeviceExtensionsKHR",
        reinterpret_cast<PFN_xrVoidFunction*>(&xrGetVulkanDeviceExtensionsKHR));

    TS_XR_CHECK(xrGetInstanceProcAddr,
        mXrInstance,
        "xrGetVulkanGraphicsRequirementsKHR",
        reinterpret_cast<PFN_xrVoidFunction*>(&xrGetVulkanGraphicsRequirementsKHR));

#ifndef NDEBUG
    TS_XR_CHECK(xrGetInstanceProcAddr,
        mXrInstance,
        "xrCreateDebugUtilsMessengerEXT",
        reinterpret_cast<PFN_xrVoidFunction*>(&xrCreateDebugUtilsMessengerEXT));

    TS_XR_CHECK(xrGetInstanceProcAddr,
        mXrInstance,
        "xrDestroyDebugUtilsMessengerEXT",
        reinterpret_cast<PFN_xrVoidFunction*>(&xrDestroyDebugUtilsMessengerEXT));
#endif // DEBUG
}

uint32_t Context::getVkGraphicsQueueFamilyIndex() const
{
    if (mVkGraphicsQueueFamilyIndex == std::nullopt)
    {
        TS_ERR("graphics queue index isn't selected yet");
    }

    return *mVkGraphicsQueueFamilyIndex;
};
uint32_t Context::getVkPresentQueueFamilyIndex() const
{
    if (mVkPresentQueueFamilyIndex == std::nullopt)
    {
        TS_ERR("present queue index isn't selected yet");
    }

    return *mVkPresentQueueFamilyIndex;
};

void Context::initXrSystemId()
{
    const XrSystemGetInfo xrSystemInfo{
        .type = XR_TYPE_SYSTEM_GET_INFO,
        .formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY
    };

    TS_XR_CHECK_MSG("No headset detected", xrGetSystem, mXrInstance, &xrSystemInfo, &mXrSystemId);
}

void Context::initXrSystemInfo()
{
    XrSystemProperties xrSystemProperties{XR_TYPE_SYSTEM_PROPERTIES};

    TS_XR_CHECK(xrGetSystemProperties, mXrInstance, mXrSystemId, &xrSystemProperties);
    
    // TODO: print xr device info
    // TODO: costum xr loader
}

void Context::isXrBlendModeAvailable()
{
    uint32_t environmentBlendModeCount;
    TS_XR_CHECK(xrEnumerateEnvironmentBlendModes,
        mXrInstance,
        mXrSystemId,
        xrViewType,
        0,
        &environmentBlendModeCount, nullptr);

    std::vector<XrEnvironmentBlendMode> supportedEnvironmentBlendModes(environmentBlendModeCount);
    TS_XR_CHECK(xrEnumerateEnvironmentBlendModes,
        mXrInstance,
        mXrSystemId,
        xrViewType,
        environmentBlendModeCount,
        &environmentBlendModeCount,
        supportedEnvironmentBlendModes.data());

    bool isModeFound{};
    for (const auto& mode : supportedEnvironmentBlendModes)
    {
        if (mode == xrEnvironmentBlendMode)
        {
            isModeFound = true;
            break;
        }
    }

    if (!isModeFound)
    {
        TS_ERR("selected XrEnvironmentBlendMode isn't supported");
    }
}

void Context::getRequiredVulkanInstanceExtensions(std::vector<std::string>& vulkanInstanceExtensions)
{
    uint32_t count;
    TS_XR_CHECK(xrGetVulkanInstanceExtensionsKHR,
        mXrInstance,
        mXrSystemId,
        0,
        &count,
        nullptr);

    std::string xrVulkanExtensionsStr(count, ' ');
    TS_XR_CHECK(xrGetVulkanInstanceExtensionsKHR,
        mXrInstance,
        mXrSystemId,
        count,
        &count,
        xrVulkanExtensionsStr.data());

    khronos_utils::unpackXrExtensionString(xrVulkanExtensionsStr, vulkanInstanceExtensions);

    vulkanInstanceExtensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
    vulkanInstanceExtensions.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

#ifndef NDEBUG
    vulkanInstanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif // DEBUG
}

void Context::isVulkanInstanceExtensionsAvailable(const std::vector<std::string>& requiredVulkanInstanceExtensions)
{
    std::vector<VkExtensionProperties> supportedVulkanInstanceExtensions;
    getSupportedVulkanInstanceExtensions(supportedVulkanInstanceExtensions);

    for (const auto& requiredExtension : requiredVulkanInstanceExtensions)
    {
        bool isExtensionSupported{};
        for (const auto& supportedExtension : supportedVulkanInstanceExtensions)
        {
            if (strcmp(supportedExtension.extensionName, requiredExtension.c_str()) == 0)
            {
                isExtensionSupported = true;
                break;
            }
        }

        if ((!isExtensionSupported) && (requiredExtension != VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
        {
            TS_ERR((requiredExtension + " extension isn't supported").c_str());
        }
        else if ((!isExtensionSupported) && requiredExtension == VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
        {
            TS_WARN(VK_EXT_DEBUG_UTILS_EXTENSION_NAME " extension isn't supported");
        }
    }
}

void Context::getSupportedVulkanInstanceExtensions(std::vector<VkExtensionProperties>& supportedVulkanInstanceExtensions)
{
    uint32_t instanceExtensionCount;
    TS_VK_CHECK(vkEnumerateInstanceExtensionProperties,
        nullptr,
        &instanceExtensionCount,
        nullptr);

    supportedVulkanInstanceExtensions.resize(instanceExtensionCount);
    TS_VK_CHECK(vkEnumerateInstanceExtensionProperties,
        nullptr,
        &instanceExtensionCount,
        supportedVulkanInstanceExtensions.data());
}

void Context::createVulkanInstance(const std::vector<std::string>& vulkanInstanceExtensions)
{
    const VkApplicationInfo applicationInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = mGameName.data(),
        .applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
        .pEngineName = ENGINE_NAME,
        .engineVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
        .apiVersion = VK_API_VERSION_1_3,
    };

    std::vector<const char*> rawVulkanInstanceExtensions;
    for (const auto& str : vulkanInstanceExtensions)
    {
        rawVulkanInstanceExtensions.emplace_back(str.c_str());
    }

    VkInstanceCreateInfo ci{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &applicationInfo,
        .enabledExtensionCount = static_cast<uint32_t>(vulkanInstanceExtensions.size()),
        .ppEnabledExtensionNames = rawVulkanInstanceExtensions.data()
    };

#ifndef NDEBUG
    std::vector<VkLayerProperties> supportedInstanceLayers;
    uint32_t instanceLayerCount;
    TS_VK_CHECK(vkEnumerateInstanceLayerProperties, &instanceLayerCount, nullptr);

    supportedInstanceLayers.resize(instanceLayerCount);
    TS_VK_CHECK(vkEnumerateInstanceLayerProperties, &instanceLayerCount, supportedInstanceLayers.data());

    for (const auto layer : vkLayers)
    {
        bool isLayerSupported{};
        for (const auto& supportedLayer : supportedInstanceLayers)
        {
            if (strcmp(layer, supportedLayer.layerName) == 0)
            {
                isLayerSupported = true;
                break;
            }
        }

        if (!isLayerSupported)
        {
            TS_WARN(("Vulkan validation layer isn't supported: "s + layer).c_str());
        }
    }

    ci.enabledLayerCount = static_cast<uint32_t>(vkLayers.size());
    ci.ppEnabledLayerNames = vkLayers.data();
#endif // DEBUG

    TS_VK_CHECK(vkCreateInstance, &ci, nullptr, &mVkInstance);
}

void Context::createPhysicalDevice()
{
    TS_XR_CHECK(xrGetVulkanGraphicsDeviceKHR, mXrInstance, mXrSystemId, mVkInstance, &mPhysicalDevice);
}

void Context::getGraphicsQueue()
{
    std::vector<VkQueueFamilyProperties> queueFamilies;
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, nullptr);

    queueFamilies.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, queueFamilies.data());

    bool drawQueueFamilyIndexFound{};
    for (size_t queueFamilyIndexCandidate{};
        queueFamilyIndexCandidate < queueFamilies.size();
        ++queueFamilyIndexCandidate)
    {
        const auto& queueFamilyCandidate = queueFamilies.at(queueFamilyIndexCandidate);

        if (queueFamilyCandidate.queueCount == 0)
        {
            continue;
        }

        if (queueFamilyCandidate.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            mVkGraphicsQueueFamilyIndex = static_cast<uint32_t>(queueFamilyIndexCandidate);
            drawQueueFamilyIndexFound = true;
            break;
        }
    }

    if (mVkGraphicsQueueFamilyIndex == std::nullopt)
    {
        TS_ERR("Graphics queue couldn't be found");
    }
}

void Context::getPresentQueue(const VkSurfaceKHR mirrorSurface)
{
    std::vector<VkQueueFamilyProperties> queueFamilies;
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, nullptr);

    queueFamilies.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, queueFamilies.data());

    for (size_t queueFamilyIndexCandidate{};
        queueFamilyIndexCandidate < queueFamilies.size();
        ++queueFamilyIndexCandidate)
    {
        const auto& queueFamilyCandidate{queueFamilies.at(queueFamilyIndexCandidate)};

        if (queueFamilyCandidate.queueCount == 0)
        {
            continue;
        }

        VkBool32 presentSupport{};
        TS_VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR,
            mPhysicalDevice,
            static_cast<uint32_t>(queueFamilyIndexCandidate),
            mirrorSurface, &presentSupport);

        if (presentSupport)
        {
            mVkPresentQueueFamilyIndex = static_cast<uint32_t>(queueFamilyIndexCandidate);
            break;
        }
    }

    if (mVkPresentQueueFamilyIndex == std::nullopt)
    {
        TS_ERR("presentation queue couldn't be found");
    }
}

void Context::isVulkanDeviceExtensionsAvailable(
    std::vector<std::string>& requiredVulkanDeviceExtensions,
    VkPhysicalDeviceFeatures& physicalDeviceFeatures,
    VkPhysicalDeviceMultiviewFeatures& physicalDeviceMultiviewFeatures,
    VkPhysicalDeviceFeatures2& physicalDeviceFeatures2)
{
    std::vector<VkExtensionProperties> supportedVulkanDeviceExtensions;
    getSupportedVulkanDeviceExtensions(supportedVulkanDeviceExtensions);

    for (auto requiredExtension : requiredVulkanDeviceExtensions)
    {
        bool isExtensionSupported = false;
        for (const auto& supportedExtension : supportedVulkanDeviceExtensions)
        {
            if (strcmp(requiredExtension.c_str(), supportedExtension.extensionName) == 0)
            {
                isExtensionSupported = true;
                break;
            }
        }

        if (!isExtensionSupported)
        {
            TS_ERR(("Vulkan extension isn't supported: " + requiredExtension).c_str());
        }
    }

    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(mPhysicalDevice, &physicalDeviceProperties);
    mVkUniformBufferOffsetAlignment = physicalDeviceProperties.limits.minUniformBufferOffsetAlignment;

    const VkSampleCountFlags sampleCountFlags{
        physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts};
    if (sampleCountFlags & VK_SAMPLE_COUNT_4_BIT)
    {
        mVkMultisampleCount = VK_SAMPLE_COUNT_4_BIT;
    }
    else if (sampleCountFlags & VK_SAMPLE_COUNT_2_BIT)
    {
        mVkMultisampleCount = VK_SAMPLE_COUNT_2_BIT;
    }

    vkGetPhysicalDeviceFeatures(mPhysicalDevice, &physicalDeviceFeatures);
    if (!physicalDeviceFeatures.shaderStorageImageMultisample)
    {
        TS_ERR("Multisampling feature isn't available");
    }

    vkGetPhysicalDeviceFeatures(mPhysicalDevice, &physicalDeviceFeatures);

    vkGetPhysicalDeviceFeatures2(mPhysicalDevice, &physicalDeviceFeatures2);
    if (!physicalDeviceMultiviewFeatures.multiview)
    {
        TS_ERR("Multview feature isn't available");
    }
}

void Context::getSupportedVulkanDeviceExtensions(std::vector<VkExtensionProperties>& vulkanDeviceExtensions)
{
    uint32_t deviceExtensionCount;
    TS_VK_CHECK(vkEnumerateDeviceExtensionProperties,
        mPhysicalDevice,
        nullptr,
        &deviceExtensionCount,
        nullptr);

    vulkanDeviceExtensions.resize(deviceExtensionCount);
    TS_VK_CHECK(vkEnumerateDeviceExtensionProperties,
        mPhysicalDevice,
        nullptr,
        &deviceExtensionCount,
        vulkanDeviceExtensions.data());
}

void Context::getRequiredVulkanDeviceExtensions(std::vector<std::string>& requiredVulkanDeviceExtensions)
{
    uint32_t count;
    TS_XR_CHECK(xrGetVulkanDeviceExtensionsKHR, mXrInstance, mXrSystemId, 0, &count, nullptr);

    std::string buffer;
    buffer.resize(count);
    TS_XR_CHECK(xrGetVulkanDeviceExtensionsKHR, mXrInstance, mXrSystemId, count, &count, buffer.data());

    khronos_utils::unpackXrExtensionString(buffer, requiredVulkanDeviceExtensions);

    requiredVulkanDeviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

void Context::createLogicalDevice(
    const std::vector<std::string>& requiredVulkanDeviceExtensions,
    const VkPhysicalDeviceFeatures& physicalDeviceFeatures,
    const VkPhysicalDeviceMultiviewFeatures& physicalDeviceMultiviewFeatures,
    std::vector<VkDeviceQueueCreateInfo>& deviceQueueCis)
{
    std::vector<const char*> requiredVulkanDeviceExtensionsPtrs;
    requiredVulkanDeviceExtensionsPtrs.reserve(requiredVulkanDeviceExtensionsPtrs.size());
    for (auto& str : requiredVulkanDeviceExtensions)
    {
        requiredVulkanDeviceExtensionsPtrs.emplace_back(str.c_str());
    }

    const VkDeviceCreateInfo deviceCi{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &physicalDeviceMultiviewFeatures,
        .queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCis.size()),
        .pQueueCreateInfos = deviceQueueCis.data(),
        .enabledExtensionCount = static_cast<uint32_t>(requiredVulkanDeviceExtensions.size()),
        .ppEnabledExtensionNames = requiredVulkanDeviceExtensionsPtrs.data(),
        .pEnabledFeatures = &physicalDeviceFeatures,
    };

    TS_VK_CHECK(vkCreateDevice, mPhysicalDevice, &deviceCi, nullptr, &mVkDevice);

    XrGraphicsRequirementsVulkanKHR graphicsRequirements{XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN_KHR};
    TS_XR_CHECK(xrGetVulkanGraphicsRequirementsKHR, mXrInstance, mXrSystemId, &graphicsRequirements);
}

void Context::createQueues(std::vector<VkDeviceQueueCreateInfo>& deviceQueueCis)
{
    constexpr float queuePriority{1.f};

    VkDeviceQueueCreateInfo deviceQueueCi {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = *mVkGraphicsQueueFamilyIndex,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
    };

    deviceQueueCis.push_back(deviceQueueCi);

    if (mVkGraphicsQueueFamilyIndex != mVkPresentQueueFamilyIndex)
    {
        deviceQueueCi.queueFamilyIndex = *mVkPresentQueueFamilyIndex;
        deviceQueueCis.push_back(deviceQueueCi);
    }
}

Context::~Context()
{
#ifndef NDEBUG
    if (mXrDebugMessenger != nullptr)
    {
        xrDestroyDebugUtilsMessengerEXT(mXrDebugMessenger);
    }
#endif // DEBUG

    if (mXrInstance != nullptr)
    {
        xrDestroyInstance(mXrInstance);
    }

#ifndef NDEBUG
    if (mVkDebugMessenger != nullptr)
    {
        vkDestroyDebugUtilsMessengerEXT(mVkInstance, mVkDebugMessenger, nullptr);
    }
#endif // NDEBUG

    if (mVkDevice != nullptr)
    {
        vkDestroyDevice(mVkDevice, nullptr);
    }

    if (mVkInstance != nullptr)
    {
        vkDestroyInstance(mVkInstance, nullptr);
    }
}

void Context::createXrInstance()
{
    TS_ASSERT_MSG(mGameName.size() > 0, "setGameName() should be firstly called");
    TS_ASSERT_MSG(mGameName.size() > 0, "setGameName() should be firstly called");

    XrApplicationInfo appInfo{
        .applicationVersion = static_cast<uint32_t>(XR_MAKE_VERSION(0, 1, 0)),
        .engineName = ENGINE_NAME,
        .engineVersion = static_cast<uint32_t>(XR_MAKE_VERSION(0, 1, 0)),
        .apiVersion = XR_CURRENT_API_VERSION,
    };

    const auto copySize = std::min(mGameName.size(), static_cast<size_t>(XR_MAX_APPLICATION_NAME_SIZE - 1));
    strncpy(appInfo.applicationName, mGameName.data(), copySize);

    std::vector<const char*> extensions{XR_KHR_VULKAN_ENABLE_EXTENSION_NAME};

#ifndef NDEBUG
    extensions.push_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif // !NDEBUG

    std::vector<XrExtensionProperties> supportedXrInstanceExtensions;

    uint32_t instanceExtensionCount;
    TS_XR_CHECK(xrEnumerateInstanceExtensionProperties, nullptr, 0, &instanceExtensionCount, nullptr)

    supportedXrInstanceExtensions.resize(instanceExtensionCount);
    for (auto& extensionProperty : supportedXrInstanceExtensions)
    {
        extensionProperty.type = XR_TYPE_EXTENSION_PROPERTIES;
    }

    TS_XR_CHECK(xrEnumerateInstanceExtensionProperties,
        nullptr,
        instanceExtensionCount,
        &instanceExtensionCount,
        supportedXrInstanceExtensions.data());

    for (const auto& extension : extensions)
    {
        bool isExtensionSupported{};
        for (const auto& supportedExtension : supportedXrInstanceExtensions)
        {
            if (strcmp(extension, supportedExtension.extensionName) == 0)
            {
                isExtensionSupported = true;
                break;
            }
        }

        if ((!isExtensionSupported) && (extension != XR_EXT_DEBUG_UTILS_EXTENSION_NAME))
        {
            TS_ERR(("OpenXr extension isn't supported: "s + extension).c_str());
        }
        else if (!isExtensionSupported)
        {
            TS_WARN(("OpenXr debug extension isn't supported: "s + extension).c_str());
        }
    }

    const XrInstanceCreateInfo instanceCi{
        .type = XR_TYPE_INSTANCE_CREATE_INFO,
        .applicationInfo = appInfo,
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .enabledExtensionNames = extensions.data(),
    };

    TS_XR_CHECK(xrCreateInstance, &instanceCi, &mXrInstance);
}
} // namespace ver
} // namespace ts