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
void Context::createContext(const std::string_view& appName)
{
    mAppName = appName;

    compileShaders("assets/shaders");

    createXrInstance();
    loadXrExtensions();

    vulkanloader::connectWithLoader();
    vulkanloader::loadExportingFunction();
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

    LOGGER_XR(xrGetInstanceProcAddr,
        mXrInstance,
        "xrCreateDebugUtilsMessengerEXT",
        reinterpret_cast<PFN_xrVoidFunction*>(&xrGetVulkanGraphicsRequirementsKHR));

#ifdef DEBUG
    LOGGER_XR(xrGetInstanceProcAddr,
        mXrInstance,
        "xrCreateDebugUtilsMessengerEXT",
        reinterpret_cast<PFN_xrVoidFunction*>(&xrCreateDebugUtilsMessengerEXT));

    LOGGER_XR(xrGetInstanceProcAddr,
        mXrInstance,
        "xrDestroyDebugUtilsMessengerEXT",
        reinterpret_cast<PFN_xrVoidFunction*>(&xrDestroyDebugUtilsMessengerEXT));
#endif

    const XrDebugUtilsMessengerCreateInfoEXT xrDebugUtilsMessengerCi{
        .type = XR_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .messageSeverities = logger::xrDebugUtilsMessageSeverityFlags,
        .messageTypes = logger::xrDebugUtilsMessageTypeFlags,
        .userCallback = reinterpret_cast<PFN_xrDebugUtilsMessengerCallbackEXT>(logger::xrCallback)
    };

    LOGGER_XR(xrCreateDebugUtilsMessengerEXT,
        mXrInstance,
        &xrDebugUtilsMessengerCi,
        &mXrDebugUtilsMessenger);
}

void Context::createXrInstance()
{
    XrApplicationInfo appInfo {
        .applicationVersion = static_cast<uint32_t>(XR_MAKE_VERSION(0, 1, 0)),
        .engineName = ENGINE_NAME,
        .engineVersion = static_cast<uint32_t>(XR_MAKE_VERSION(0, 1, 0)),
        .apiVersion = XR_CURRENT_API_VERSION,
    };

    if (mAppName.length() > XR_MAX_APPLICATION_NAME_SIZE - 1)
    {
        LOGGER_WARN(
            "length of the game name has been reduced to the sie of XR_MAX_APPLICATION_NAME_SIZE,"
            "which is " STR(XR_MAX_APPLICATION_NAME_SIZE) );
    }

    mAppName.copy(appInfo.applicationName,
        std::min(mAppName.length(), static_cast<std::size_t>(XR_MAX_APPLICATION_NAME_SIZE - 1)));

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

    LOGGER_XR(
        xrEnumerateInstanceExtensionProperties,
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