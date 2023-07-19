#include "context.h"
#include "tsengine/logger.h"

namespace
{
    PFN_xrGetVulkanInstanceExtensionsKHR xrGetVulkanInstanceExtensionsKHR{};
    PFN_xrGetVulkanGraphicsDeviceKHR xrGetVulkanGraphicsDeviceKHR{};
    PFN_xrGetVulkanDeviceExtensionsKHR xrGetVulkanDeviceExtensionsKHR{};
    PFN_xrGetVulkanGraphicsRequirementsKHR xrGetVulkanGraphicsRequirementsKHR{};

#ifndef NDEBUG
    PFN_xrCreateDebugUtilsMessengerEXT xrCreateDebugUtilsMessengerEXT{};
    PFN_xrDestroyDebugUtilsMessengerEXT xrDestroyDebugUtilsMessengerEXT{};
#endif // DEBUG
} // namespace

namespace ts
{
void Context::createDevice(VkSurfaceKHR pMirrorSurface)
{
    createPhysicalDevice();
    getGraphicsQueue();
    getPresentQueue(pMirrorSurface);

    std::vector<std::string> requiredVulkanDeviceExtensions;
    getRequiredVulkanDeviceExtensions(requiredVulkanDeviceExtensions);

    VkPhysicalDeviceFeatures physicalDeviceFeatures;
    VkPhysicalDeviceMultiviewFeatures physicalDeviceMultiviewFeatures{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES };
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
}

void Context::loadXrExtensions()
{
    LOGGER_XR(xrGetInstanceProcAddr,
        mpXrInstance,
        "xrGetVulkanInstanceExtensionsKHR",
        reinterpret_cast<PFN_xrVoidFunction*>(&xrGetVulkanInstanceExtensionsKHR));

    LOGGER_XR(xrGetInstanceProcAddr,
        mpXrInstance,
        "xrGetVulkanGraphicsDeviceKHR",
        reinterpret_cast<PFN_xrVoidFunction*>(&xrGetVulkanGraphicsDeviceKHR));

    LOGGER_XR(xrGetInstanceProcAddr,
        mpXrInstance,
        "xrGetVulkanDeviceExtensionsKHR",
        reinterpret_cast<PFN_xrVoidFunction*>(&xrGetVulkanDeviceExtensionsKHR));

#ifndef NDEBUG
    LOGGER_XR(xrGetInstanceProcAddr,
        mpXrInstance,
        "xrCreateDebugUtilsMessengerEXT",
        reinterpret_cast<PFN_xrVoidFunction*>(&xrCreateDebugUtilsMessengerEXT));

    LOGGER_XR(xrGetInstanceProcAddr,
        mpXrInstance,
        "xrDestroyDebugUtilsMessengerEXT",
        reinterpret_cast<PFN_xrVoidFunction*>(&xrDestroyDebugUtilsMessengerEXT));
#endif // DEBUG
}

#ifndef NDEBUG
void Context::createXrDebugMessenger()
{
    const XrDebugUtilsMessengerCreateInfoEXT ci{
        .type = XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverities = logger::xrDebugMessageSeverityFlags,
        .messageTypes = logger::xrDebugMessageTypeFlags,
        .userCallback = reinterpret_cast<PFN_xrDebugUtilsMessengerCallbackEXT>(logger::xrCallback)
    };

    LOGGER_XR(xrCreateDebugUtilsMessengerEXT,
        mpXrInstance,
        &ci,
        &mpXrDebugMessenger);
}
#endif // DEBUG

void Context::initXrSystemId()
{
    const XrSystemGetInfo ci {
        .type = XR_TYPE_SYSTEM_GET_INFO,
        .formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY
    };

    auto result{ xrGetSystem(mpXrInstance, &ci, &mXrSystemId) };

    if (XR_FAILED(result))
    {
        LOGGER_ERR("no headset detected");
    }
}

void Context::isXrBlendModeAvailable()
{
    uint32_t environmentBlendModeCount;
    LOGGER_XR(xrEnumerateEnvironmentBlendModes,
        mpXrInstance,
        mXrSystemId,
        xrViewType,
        0u,
        &environmentBlendModeCount, nullptr);

    std::vector<XrEnvironmentBlendMode> supportedEnvironmentBlendModes(environmentBlendModeCount);
    LOGGER_XR(xrEnumerateEnvironmentBlendModes,
        mpXrInstance,
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
        mpXrInstance,
        mXrSystemId,
        0u,
        &count,
        nullptr);

    std::string xrVulkanExtensionsStr(count, ' ');
    LOGGER_XR(xrGetVulkanInstanceExtensionsKHR,
        mpXrInstance,
        mXrSystemId,
        count,
        &count,
        xrVulkanExtensionsStr.data());

    utils::unpackXrExtensionString(xrVulkanExtensionsStr, vulkanInstanceExtensions);

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
    const VkApplicationInfo applicationInfo {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = GAME_NAME,
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

    VkInstanceCreateInfo ci {
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
        for (const VkLayerProperties& supportedLayer : supportedInstanceLayers)
        {
            if (strcmp(layer, supportedLayer.layerName) == 0)
            {
                isLayerSupported = true;
                break;
            }
        }

        if (!isLayerSupported)
        {
            LOGGER_WARN((std::string{ layer } + " vulkan layer isn't supported").c_str());
        }
    }

    ci.enabledLayerCount = static_cast<uint32_t>(vkLayers.size());
    ci.ppEnabledLayerNames = vkLayers.data();
#endif // DEBUG

    LOGGER_VK(vkCreateInstance, &ci, nullptr, &mpVkInstance);
}

void Context::createPhysicalDevice()
{
    LOGGER_XR(xrGetVulkanGraphicsDeviceKHR, mpXrInstance, mXrSystemId, mpVkInstance, &mpPhysicalDevice);
}

void Context::getGraphicsQueue()
{
    std::vector<VkQueueFamilyProperties> queueFamilies;
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(mpPhysicalDevice, &queueFamilyCount, nullptr);

    queueFamilies.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(mpPhysicalDevice, &queueFamilyCount, queueFamilies.data());

    bool drawQueueFamilyIndexFound{};
    for (size_t queueFamilyIndexCandidate{};
        queueFamilyIndexCandidate < queueFamilies.size();
        ++queueFamilyIndexCandidate)
    {
        const auto& queueFamilyCandidate{ queueFamilies.at(queueFamilyIndexCandidate) };

        if (queueFamilyCandidate.queueCount == 0u)
        {
            continue;
        }

        if (queueFamilyCandidate.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            mpGraphicsQueueFamilyIndex = static_cast<uint32_t>(queueFamilyIndexCandidate);
            drawQueueFamilyIndexFound = true;
            break;
        }
    }

    if (mpGraphicsQueueFamilyIndex == std::nullopt)
    {
        LOGGER_ERR("graphics queue couldn't be found");
    }
}

void Context::getPresentQueue(VkSurfaceKHR pMirrorSurface)
{
    std::vector<VkQueueFamilyProperties> queueFamilies;
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(mpPhysicalDevice, &queueFamilyCount, nullptr);

    queueFamilies.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(mpPhysicalDevice, &queueFamilyCount, queueFamilies.data());

    for (size_t queueFamilyIndexCandidate{};
        queueFamilyIndexCandidate < queueFamilies.size();
        ++queueFamilyIndexCandidate)
    {
        const auto& queueFamilyCandidate{ queueFamilies.at(queueFamilyIndexCandidate) };

        if (queueFamilyCandidate.queueCount == 0u)
        {
            continue;
        }

        VkBool32 presentSupport{};
        LOGGER_VK(vkGetPhysicalDeviceSurfaceSupportKHR,
            mpPhysicalDevice,
            static_cast<uint32_t>(queueFamilyIndexCandidate),
            pMirrorSurface, &presentSupport);

        if (presentSupport)
        {
            mpPresentQueueFamilyIndex = static_cast<uint32_t>(queueFamilyIndexCandidate);
            break;
        }
    }

    if (mpPresentQueueFamilyIndex == std::nullopt)
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
            LOGGER_ERR((requiredExtension + " extension isn't supported").c_str());
        }
    }

    vkGetPhysicalDeviceFeatures(mpPhysicalDevice, &physicalDeviceFeatures);

    vkGetPhysicalDeviceFeatures2(mpPhysicalDevice, &physicalDeviceFeatures2);
    if (!physicalDeviceMultiviewFeatures.multiview)
    {
        LOGGER_ERR("multview extension isn't available");
    }
}

void Context::getSupportedVulkanDeviceExtensions(std::vector<VkExtensionProperties>& vulkanDeviceExtensions)
{
    uint32_t deviceExtensionCount;
    LOGGER_VK(vkEnumerateDeviceExtensionProperties,
        mpPhysicalDevice,
        nullptr,
        &deviceExtensionCount,
        nullptr);

    vulkanDeviceExtensions.resize(deviceExtensionCount);
    LOGGER_VK(vkEnumerateDeviceExtensionProperties,
        mpPhysicalDevice,
        nullptr,
        &deviceExtensionCount,
        vulkanDeviceExtensions.data());
}

void Context::getRequiredVulkanDeviceExtensions(std::vector<std::string>& requiredVulkanDeviceExtensions)
{
    uint32_t count;
    LOGGER_XR(xrGetVulkanDeviceExtensionsKHR, mpXrInstance, mXrSystemId, 0u, &count, nullptr);

    std::string buffer;
    buffer.resize(count);
    LOGGER_XR(xrGetVulkanDeviceExtensionsKHR, mpXrInstance, mXrSystemId, count, &count, buffer.data());

    utils::unpackXrExtensionString(buffer, requiredVulkanDeviceExtensions);

    requiredVulkanDeviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

void Context::createLogicalDevice(
    const std::vector<std::string>& requiredVulkanDeviceExtensions,
    const VkPhysicalDeviceFeatures& physicalDeviceFeatures,
    const VkPhysicalDeviceMultiviewFeatures& physicalDeviceMultiviewFeatures,
    std::vector<VkDeviceQueueCreateInfo>& deviceQueueCis)
{
    std::vector<const char*> requiredVulkanDeviceExtensionsPtrs(requiredVulkanDeviceExtensions.size());
    for (auto& str : requiredVulkanDeviceExtensions)
    {
        requiredVulkanDeviceExtensionsPtrs.emplace_back(str.c_str());
    }

    VkDeviceCreateInfo deviceCi{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &physicalDeviceMultiviewFeatures,
        .queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCis.size()),
        .pQueueCreateInfos = deviceQueueCis.data(),
        .enabledExtensionCount = static_cast<uint32_t>(requiredVulkanDeviceExtensions.size()),
        .ppEnabledExtensionNames = requiredVulkanDeviceExtensionsPtrs.data(),
        .pEnabledFeatures = &physicalDeviceFeatures,
    };

    LOGGER_VK(vkCreateDevice, mpPhysicalDevice, &deviceCi, nullptr, &mpDevice);
}

void Context::createQueues(std::vector<VkDeviceQueueCreateInfo>& deviceQueueCis)
{
    constexpr float queuePriority = 1.0f;

    VkDeviceQueueCreateInfo deviceQueueCi {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = *mpGraphicsQueueFamilyIndex,
        .queueCount = 1u,
        .pQueuePriorities = &queuePriority
    };

    deviceQueueCis.push_back(deviceQueueCi);

    if (mpGraphicsQueueFamilyIndex != mpPresentQueueFamilyIndex)
    {
        deviceQueueCi.queueFamilyIndex = *mpPresentQueueFamilyIndex;
        deviceQueueCis.push_back(deviceQueueCi);
    }
}

#ifndef NDEBUG
void Context::createVkDebugMessenger(const VkInstance instance)
{
    const VkDebugUtilsMessengerCreateInfoEXT ci {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverity = logger::vkDebugMessageSeverityFlags,
        .messageType = logger::vkDebugMessageTypeFlags,
        .pfnUserCallback = &logger::vkCallback
    };

    LOGGER_VK(vkCreateDebugUtilsMessengerEXT, instance, &ci, nullptr, &mpVkDebugMessenger);
}
#endif

Context::~Context()
{
#ifndef NDEBUG
    if (mpXrDebugMessenger != nullptr)
    {
        xrDestroyDebugUtilsMessengerEXT(mpXrDebugMessenger);
    }
#endif // DEBUG

    if (mpXrInstance != nullptr)
    {
        xrDestroyInstance(mpXrInstance);
    }

#ifndef NDEBUG
    if (mpVkDebugMessenger != nullptr)
    {
        vkDestroyDebugUtilsMessengerEXT(mpVkInstance, mpVkDebugMessenger, nullptr);
    }
#endif // DEBUG

    if (mpVkInstance != nullptr)
    {
        vkDestroyInstance(mpVkInstance, nullptr);
    }
}

void Context::createOpenXrContext()
{
    createXrInstance();
    loadXrExtensions();
#ifndef NDEBUG
    createXrDebugMessenger();
#endif
    initXrSystemId();
    isXrBlendModeAvailable();
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

    vulkanloader::loadInstanceLevelFunctions(mpVkInstance, vulkanInstanceExtensions);

#ifdef _DEBUG
    vulkanloader::loadDebugLevelFunctions(mpVkInstance);
    createVkDebugMessenger(mpVkInstance);
#endif
}

void Context::createXrInstance()
{
    XrApplicationInfo appInfo {
        .applicationName = GAME_NAME,
        .applicationVersion = static_cast<uint32_t>(XR_MAKE_VERSION(0, 1, 0)),
        .engineName = ENGINE_NAME,
        .engineVersion = static_cast<uint32_t>(XR_MAKE_VERSION(0, 1, 0)),
        .apiVersion = XR_CURRENT_API_VERSION,
    };

    if (strlen(GAME_NAME) > XR_MAX_APPLICATION_NAME_SIZE - 1)
    {
        LOGGER_WARN("length of the game name has been reduced to the sie of XR_MAX_APPLICATION_NAME_SIZE,"
            "which is " STR(XR_MAX_APPLICATION_NAME_SIZE) );
    }

    std::vector<const char*> extensions{ XR_KHR_VULKAN_ENABLE_EXTENSION_NAME };

#ifndef NDEBUG
    extensions.push_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif // DEBUG

    std::vector<XrExtensionProperties> supportedXrInstanceExtensions;

    uint32_t instanceExtensionCount;
    LOGGER_XR(xrEnumerateInstanceExtensionProperties, nullptr, 0u, &instanceExtensionCount, nullptr)

    supportedXrInstanceExtensions.resize(instanceExtensionCount);
    for (XrExtensionProperties& extensionProperty : supportedXrInstanceExtensions)
    {
        extensionProperty.type = XR_TYPE_EXTENSION_PROPERTIES;
        extensionProperty.next = nullptr;
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

        if (!isExtensionSupported)
        {
            LOGGER_WARN((extension + std::string{ " xr extension isn't supported" }).c_str());
        }
    }

    const XrInstanceCreateInfo instanceCi {
        .type = XR_TYPE_INSTANCE_CREATE_INFO,
        .applicationInfo = appInfo,
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .enabledExtensionNames = extensions.data(),
    };

    LOGGER_XR(xrCreateInstance, &instanceCi, &mpXrInstance);
}
} // namespace ts