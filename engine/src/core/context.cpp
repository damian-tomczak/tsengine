#include "context.h"
#include "tsengine/logger.h"

namespace
{
    PFN_xrGetVulkanInstanceExtensionsKHR xrGetVulkanInstanceExtensionsKHR{};
    PFN_xrGetVulkanGraphicsDeviceKHR xrGetVulkanGraphicsDeviceKHR{};
    PFN_xrGetVulkanDeviceExtensionsKHR xrGetVulkanDeviceExtensionsKHR{};
    PFN_xrGetVulkanGraphicsRequirementsKHR xrGetVulkanGraphicsRequirementsKHR{};

#ifdef DEBUG
    PFN_xrCreateDebugUtilsMessengerEXT xrCreateDebugUtilsMessengerEXT{};
    PFN_xrDestroyDebugUtilsMessengerEXT xrDestroyDebugUtilsMessengerEXT{};
#endif
}

namespace ts
{
void Context::createContext()
{
    compileShaders("assets/shaders");

    createXrInstance();
    loadXrExtensions();
    initXrSystemId();
    checkAvailabilityXrBlendMode();

    vulkanloader::connectWithLoader();
    vulkanloader::loadExportingFunction();
    vulkanloader::loadGlobalLevelFunctions();

    std::vector<std::string> vulkanInstanceExtensions;
    getRequiredVkInstanceExtensionsAndCheckAvailability(vulkanInstanceExtensions);

    createVkInstance(std::move(vulkanInstanceExtensions));
}

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

#ifdef DEBUG
    LOGGER_XR(xrGetInstanceProcAddr,
        mXrInstance,
        "xrCreateDebugUtilsMessengerEXT",
        reinterpret_cast<PFN_xrVoidFunction*>(&xrCreateDebugUtilsMessengerEXT));

    LOGGER_XR(xrGetInstanceProcAddr,
        mXrInstance,
        "xrDestroyDebugUtilsMessengerEXT",
        reinterpret_cast<PFN_xrVoidFunction*>(&xrDestroyDebugUtilsMessengerEXT));

    const XrDebugUtilsMessengerCreateInfoEXT xrDebugUtilsMessengerCi {
        .type = XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverities = logger::xrDebugUtilsMessageSeverityFlags,
        .messageTypes = logger::xrDebugUtilsMessageTypeFlags,
        .userCallback = reinterpret_cast<PFN_xrDebugUtilsMessengerCallbackEXT>(logger::xrCallback)
    };

    LOGGER_XR(xrCreateDebugUtilsMessengerEXT,
        mXrInstance,
        &xrDebugUtilsMessengerCi,
        &mXrDebugUtilsMessenger);
#endif
}

void Context::initXrSystemId()
{
    const XrSystemGetInfo ci {
        .type = XR_TYPE_SYSTEM_GET_INFO,
        .formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY
    };

    auto result{ xrGetSystem(mXrInstance, &ci, &mXrSystemId) };
    if (XR_FAILED(result))
    {
        LOGGER_ERR("no headset detected");
    }
}

void Context::checkAvailabilityXrBlendMode()
{
    uint32_t environmentBlendModeCount;
    LOGGER_XR(xrEnumerateEnvironmentBlendModes,
        mXrInstance,
        mXrSystemId,
        xrViewType,
        0u,
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

void Context::getRequiredVkInstanceExtensionsAndCheckAvailability(std::vector<std::string>& requiredVkInstanceExtensions)
{
    uint32_t count;
    LOGGER_XR(xrGetVulkanInstanceExtensionsKHR,
        mXrInstance,
        mXrSystemId,
        0u,
        &count,
        nullptr);

    std::string xrVulkanExtensions(count, ' ');
    LOGGER_XR(xrGetVulkanInstanceExtensionsKHR,
        mXrInstance,
        mXrSystemId,
        count,
        &count,
        xrVulkanExtensions.data());

    std::ranges::move(utils::unpackExtensionString(xrVulkanExtensions),
        std::back_inserter(requiredVkInstanceExtensions));

#ifdef _DEBUG
    requiredVkInstanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    std::vector<VkExtensionProperties> supportedVulkanInstanceExtensions;
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

    for (const auto& extension : requiredVkInstanceExtensions)
    {
        bool isExtensionSupported{};
        for (const auto& supportedExtension : supportedVulkanInstanceExtensions)
        {
            if (strcmp(supportedExtension.extensionName, extension.c_str()) == 0)
            {
                isExtensionSupported = true;
                break;
            }
        }

        if ((!isExtensionSupported) && (extension != VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
        {
            LOGGER_ERR((extension + " isn't supported").c_str());
        }
        else if ((!isExtensionSupported) && extension == VK_EXT_DEBUG_UTILS_EXTENSION_NAME)
        {
            LOGGER_WARN(VK_EXT_DEBUG_UTILS_EXTENSION_NAME " isn't supported");
        }
    }
}

void Context::createVkInstance(std::vector<std::string>&& vulkanInstanceExtensions)
{
    const VkApplicationInfo applicationInfo {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = GAME_NAME,
        .applicationVersion = VK_MAKE_API_VERSION(0, 0, 1, 0),
        .pEngineName = "OpenXR Vulkan Example",
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

#ifdef DEBUG
    std::vector<VkLayerProperties> supportedInstanceLayers;
    uint32_t instanceLayerCount;
    LOGGER_VK(vkEnumerateInstanceLayerProperties,
        &instanceLayerCount, nullptr);

    supportedInstanceLayers.resize(instanceLayerCount);
    LOGGER_VK(vkEnumerateInstanceLayerProperties,
        &instanceLayerCount,
        supportedInstanceLayers.data());

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
#endif

    LOGGER_VK(vkCreateInstance,
        &ci,
        nullptr,
        &mVkInstance);
}

Context::~Context()
{
#ifdef DEBUG
    if (mXrDebugUtilsMessenger)
    {
        xrDestroyDebugUtilsMessengerEXT(mXrDebugUtilsMessenger);
    }
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

#ifdef DEBUG
    extensions.push_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

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

    LOGGER_XR(xrCreateInstance, &instanceCi, &mXrInstance);
}
}