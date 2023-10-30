#include "context.h"
#include "khronos_utils.h"
#include "openxr/openxr_platform.h"
#include "vulkan_tools/vulkan_loader.h"
#include "globals.hpp"

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

namespace ts
{
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

    return *this;
}

void Context::createVulkanContext()
{
    vulkanloader::connectWithLoader();
    vulkanloader::loadExportFunction();
    vulkanloader::loadGlobalLevelFunctions();

    std::vector<std::string> vulkanInstanceExtensions;
    getRequiredVulkanInstanceExtensions(vulkanInstanceExtensions);
    isVulkanInstanceExtensionsAvailable(vulkanInstanceExtensions);

    createVulkanInstance(vulkanInstanceExtensions);

    vulkanloader::loadInstanceLevelFunctions(mVkInstance, vulkanInstanceExtensions);

#ifdef _DEBUG
    vulkanloader::loadDebugLevelFunctions(mVkInstance);
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

    vulkanloader::loadDeviceLevelFunctions(mVkDevice, requiredVulkanDeviceExtensions);

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

    LOGGER_XR(xrCreateDebugUtilsMessengerEXT,
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

    LOGGER_VK(vkCreateDebugUtilsMessengerEXT, mVkInstance, &ci, nullptr, &mVkDebugMessenger);
}
#endif // DEBUG

void Context::loadXrExtensions()
{
    LOGGER_XR(xrGetInstanceProcAddr,
        mXrInstance,
        "xrGetVulkanInstanceExtensionsKHR",
        reinterpret_cast<PFN_xrVoidFunction*>(&xrGetVulkanInstanceExtensionsKHR));

    LOGGER_XR(xrGetInstanceProcAddr,
        mXrInstance,
        "xrGetVulkanGraphicsDeviceKHR",
        reinterpret_cast<PFN_xrVoidFunction*>(&xrGetVulkanGraphicsDeviceKHR));

    LOGGER_XR(xrGetInstanceProcAddr,
        mXrInstance,
        "xrGetVulkanDeviceExtensionsKHR",
        reinterpret_cast<PFN_xrVoidFunction*>(&xrGetVulkanDeviceExtensionsKHR));

    LOGGER_XR(xrGetInstanceProcAddr,
        mXrInstance,
        "xrGetVulkanGraphicsRequirementsKHR",
        reinterpret_cast<PFN_xrVoidFunction*>(&xrGetVulkanGraphicsRequirementsKHR));

#ifndef NDEBUG
    LOGGER_XR(xrGetInstanceProcAddr,
        mXrInstance,
        "xrCreateDebugUtilsMessengerEXT",
        reinterpret_cast<PFN_xrVoidFunction*>(&xrCreateDebugUtilsMessengerEXT));

    LOGGER_XR(xrGetInstanceProcAddr,
        mXrInstance,
        "xrDestroyDebugUtilsMessengerEXT",
        reinterpret_cast<PFN_xrVoidFunction*>(&xrDestroyDebugUtilsMessengerEXT));
#endif // DEBUG
}

uint32_t Context::getVkGraphicsQueueFamilyIndex() const
{
    if (mVkGraphicsQueueFamilyIndex == std::nullopt)
    {
        LOGGER_ERR("graphics queue index isn't selected yet");
    }

    return *mVkGraphicsQueueFamilyIndex;
};
uint32_t Context::getVkPresentQueueFamilyIndex() const
{
    if (mVkPresentQueueFamilyIndex == std::nullopt)
    {
        LOGGER_ERR("present queue index isn't selected yet");
    }

    return *mVkPresentQueueFamilyIndex;
};

void Context::initXrSystemId()
{
    const XrSystemGetInfo xrSystemInfo{
        .type = XR_TYPE_SYSTEM_GET_INFO,
        .formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY
    };

    auto result = xrGetSystem(mXrInstance, &xrSystemInfo, &mXrSystemId);

    if (XR_FAILED(result))
    {
        LOGGER_ERR("No headset detected");
    }
}

void Context::initXrSystemInfo()
{
    XrSystemProperties xrSystemProperties{XR_TYPE_SYSTEM_PROPERTIES};

    LOGGER_XR(xrGetSystemProperties, mXrInstance, mXrSystemId, &xrSystemProperties);

    auto knownDeviceIt = std::ranges::find_if(khronos_utils::knownXrDevices, [xrSystemProperties](const auto& systemId) -> bool {
        return systemId == xrSystemProperties.systemId;
    });

    if (knownDeviceIt == khronos_utils::knownXrDevices.end())
    {
        LOGGER_WARN("Engine doesn't recognize headset in use");
        return;
    }

    auto knownUnstableDeviceIt = std::ranges::find_if(khronos_utils::knownUnstableXrDevices, [xrSystemProperties](const auto& systemId) -> bool {
        return systemId == xrSystemProperties.systemId;
    });

    if (knownUnstableDeviceIt != khronos_utils::knownUnstableXrDevices.end())
    {
        khronos_utils::gThrowExceptions = false;
        LOGGER_WARN(std::format(
            "Unstable OpenXr device in use."
            "\nSystemId: {} ({})\n"
            "Vulkan validation layers' errors won't stop the program.",
            xrSystemProperties.systemId, xrSystemProperties.systemName).c_str());
        return;
    }
}

void Context::isXrBlendModeAvailable()
{
    uint32_t environmentBlendModeCount;
    LOGGER_XR(xrEnumerateEnvironmentBlendModes,
        mXrInstance,
        mXrSystemId,
        xrViewType,
        0,
        &environmentBlendModeCount, nullptr);

    std::vector<XrEnvironmentBlendMode> supportedEnvironmentBlendModes(environmentBlendModeCount);
    LOGGER_XR(xrEnumerateEnvironmentBlendModes,
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
        LOGGER_ERR("selected XrEnvironmentBlendMode isn't supported");
    }
}

void Context::getRequiredVulkanInstanceExtensions(std::vector<std::string>& vulkanInstanceExtensions)
{
    uint32_t count;
    LOGGER_XR(xrGetVulkanInstanceExtensionsKHR,
        mXrInstance,
        mXrSystemId,
        0,
        &count,
        nullptr);

    std::string xrVulkanExtensionsStr(count, ' ');
    LOGGER_XR(xrGetVulkanInstanceExtensionsKHR,
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
            LOGGER_ERR((requiredExtension + " extension isn't supported").c_str());
        }
        else if ((!isExtensionSupported) && requiredExtension == VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
        {
            LOGGER_WARN(VK_EXT_DEBUG_UTILS_EXTENSION_NAME " extension isn't supported");
        }
    }
}

void Context::getSupportedVulkanInstanceExtensions(std::vector<VkExtensionProperties>& supportedVulkanInstanceExtensions)
{
    uint32_t instanceExtensionCount;
    LOGGER_VK(vkEnumerateInstanceExtensionProperties,
        nullptr,
        &instanceExtensionCount,
        nullptr);

    supportedVulkanInstanceExtensions.resize(instanceExtensionCount);
    LOGGER_VK(vkEnumerateInstanceExtensionProperties,
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
    LOGGER_VK(vkEnumerateInstanceLayerProperties, &instanceLayerCount, nullptr);

    supportedInstanceLayers.resize(instanceLayerCount);
    LOGGER_VK(vkEnumerateInstanceLayerProperties, &instanceLayerCount, supportedInstanceLayers.data());

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
            LOGGER_WARN(("Vulkan validation layer isn't supported: "s + layer).c_str());
        }
    }

    ci.enabledLayerCount = static_cast<uint32_t>(vkLayers.size());
    ci.ppEnabledLayerNames = vkLayers.data();
#endif // DEBUG

    LOGGER_VK(vkCreateInstance, &ci, nullptr, &mVkInstance);
}

void Context::createPhysicalDevice()
{
    LOGGER_XR(xrGetVulkanGraphicsDeviceKHR, mXrInstance, mXrSystemId, mVkInstance, &mPhysicalDevice);
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
        LOGGER_ERR("Graphics queue couldn't be found");
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
        LOGGER_VK(vkGetPhysicalDeviceSurfaceSupportKHR,
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
        LOGGER_ERR("presentation queue couldn't be found");
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
            LOGGER_ERR(("Vulkan extension isn't supported: " + requiredExtension).c_str());
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
        LOGGER_ERR("Multisampling feature isn't available");
    }

    vkGetPhysicalDeviceFeatures(mPhysicalDevice, &physicalDeviceFeatures);

    vkGetPhysicalDeviceFeatures2(mPhysicalDevice, &physicalDeviceFeatures2);
    if (!physicalDeviceMultiviewFeatures.multiview)
    {
        LOGGER_ERR("Multview feature isn't available");
    }
}

void Context::getSupportedVulkanDeviceExtensions(std::vector<VkExtensionProperties>& vulkanDeviceExtensions)
{
    uint32_t deviceExtensionCount;
    LOGGER_VK(vkEnumerateDeviceExtensionProperties,
        mPhysicalDevice,
        nullptr,
        &deviceExtensionCount,
        nullptr);

    vulkanDeviceExtensions.resize(deviceExtensionCount);
    LOGGER_VK(vkEnumerateDeviceExtensionProperties,
        mPhysicalDevice,
        nullptr,
        &deviceExtensionCount,
        vulkanDeviceExtensions.data());
}

void Context::getRequiredVulkanDeviceExtensions(std::vector<std::string>& requiredVulkanDeviceExtensions)
{
    uint32_t count;
    LOGGER_XR(xrGetVulkanDeviceExtensionsKHR, mXrInstance, mXrSystemId, 0, &count, nullptr);

    std::string buffer;
    buffer.resize(count);
    LOGGER_XR(xrGetVulkanDeviceExtensionsKHR, mXrInstance, mXrSystemId, count, &count, buffer.data());

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

    LOGGER_VK(vkCreateDevice, mPhysicalDevice, &deviceCi, nullptr, &mVkDevice);

    XrGraphicsRequirementsVulkanKHR graphicsRequirements{XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN_KHR};
    LOGGER_XR(xrGetVulkanGraphicsRequirementsKHR, mXrInstance, mXrSystemId, &graphicsRequirements);
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
    LOGGER_XR(xrEnumerateInstanceExtensionProperties, nullptr, 0, &instanceExtensionCount, nullptr)

    supportedXrInstanceExtensions.resize(instanceExtensionCount);
    for (auto& extensionProperty : supportedXrInstanceExtensions)
    {
        extensionProperty.type = XR_TYPE_EXTENSION_PROPERTIES;
    }

    LOGGER_XR(xrEnumerateInstanceExtensionProperties,
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
            LOGGER_ERR(("OpenXr extension isn't supported: "s + extension).c_str());
        }
        else if (!isExtensionSupported)
        {
            LOGGER_WARN(("OpenXr debug extension isn't supported: "s + extension).c_str());
        }
    }

    const XrInstanceCreateInfo instanceCi{
        .type = XR_TYPE_INSTANCE_CREATE_INFO,
        .applicationInfo = appInfo,
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .enabledExtensionNames = extensions.data(),
    };

    LOGGER_XR(xrCreateInstance, &instanceCi, &mXrInstance);
}
} // namespace ts