#include "context.h"

namespace ts
{
Context::Context(const std::string_view& appName) : mAppName{ appName }
{
    compileShaders("assets/shaders");

    createOpenXRInstance();

    vulkanloader::connectWithLoader();
    vulkanloader::loadExportingFunction();
}

void Context::createOpenXRInstance()
{
    XrApplicationInfo appInfo
    {
        .applicationVersion{ static_cast<uint32_t>(XR_MAKE_VERSION(0, 1, 0)) },
        .engineName{ ENGINE_NAME },
        .engineVersion{ static_cast<uint32_t>(XR_MAKE_VERSION(0, 1, 0)) },
        .apiVersion{ XR_CURRENT_API_VERSION },
    };

    if (mAppName.length() > XR_MAX_APPLICATION_NAME_SIZE - 1)
    {
        // TODO: logger
    }

    mAppName.copy(appInfo.applicationName,
        std::min(mAppName.length(), static_cast<std::size_t>(XR_MAX_APPLICATION_NAME_SIZE - 1)));

    std::vector<const char*> extensions{ XR_KHR_VULKAN_ENABLE_EXTENSION_NAME };

#ifdef _DEBUG
    extensions.push_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    std::vector<XrExtensionProperties> supportedXrInstanceExtensions;

    uint32_t instanceExtensionCount;
    if (!xrEnumerateInstanceExtensionProperties(nullptr, 0u, &instanceExtensionCount, nullptr))
    {
        // TODO: logger
    }

    supportedXrInstanceExtensions.resize(instanceExtensionCount);
    for (XrExtensionProperties& extensionProperty : supportedXrInstanceExtensions)
    {
        extensionProperty.type = XR_TYPE_EXTENSION_PROPERTIES;
        extensionProperty.next = nullptr;
    }

    if (!xrEnumerateInstanceExtensionProperties(
        nullptr,
        instanceExtensionCount,
        &instanceExtensionCount,
        supportedXrInstanceExtensions.data()))
    {
        // TODO: logger
    }

    for (const auto& extension : extensions)
    {
        for (const XrExtensionProperties& supportedExtension : supportedXrInstanceExtensions)
        {
            if (extension == supportedExtension.extensionName)
            {
                // TODO: logger
                break;
            }
        }
    }

    const XrInstanceCreateInfo instanceCi
    {
        .type{ XR_TYPE_INSTANCE_CREATE_INFO },
        .applicationInfo{ appInfo },
        .enabledExtensionCount{ static_cast<uint32_t>(extensions.size())},
        .enabledExtensionNames{ extensions.data() },
    };

    if (!xrCreateInstance(&instanceCi, &mXrInstance))
    {
        puts("123");
        // TODO: logger
    }
}
} // namespace ts