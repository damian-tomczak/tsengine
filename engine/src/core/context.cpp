#include "context.h"

namespace ts
{
Context::Context()
{
    compileShaders("assets/shaders");

    createOpenXRInstance();

    vulkanloader::connectWithLoader();
    vulkanloader::loadExportingFunction();
}

void Context::createOpenXRInstance()
{
    XrApplicationInfo applicationInfo;
    applicationInfo.apiVersion = XR_CURRENT_API_VERSION;
    applicationInfo.applicationVersion = static_cast<uint32_t>(XR_MAKE_VERSION(0, 1, 0));
    applicationInfo.engineVersion = static_cast<uint32_t>(XR_MAKE_VERSION(0, 1, 0));

    memcpy(applicationInfo.applicationName, "", 0);
    memcpy(applicationInfo.engineName, "", 0);

    std::vector<const char*> extensions = { "" };

#ifdef _DEBUG
    extensions.push_back(XR_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    // Check that all OpenXR instance extensions are supported
    for (const char* extension : extensions)
    {
        bool extensionSupported = false;
        for (const XrExtensionProperties& supportedExtension : std::vector<XrExtensionProperties>{})
        {
            if (strcmp(extension, supportedExtension.extensionName) == 0)
            {
                extensionSupported = true;
                break;
            }
        }

        if (!extensionSupported)
        {
            std::stringstream s;
            s << "OpenXR instance extension \"" << extension << "\"";
            //util::error(Error::FeatureNotSupported, s.str());
            //valid = false;
            return;
        }
    }

    XrInstanceCreateInfo instanceCreateInfo{ XR_TYPE_INSTANCE_CREATE_INFO };
    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instanceCreateInfo.enabledExtensionNames = extensions.data();
    instanceCreateInfo.applicationInfo = applicationInfo;
    const XrResult result = xrCreateInstance(&instanceCreateInfo, nullptr);
    if (XR_FAILED(result))
    {
        //util::error(Error::HeadsetNotConnected);
        //valid = false;
        return;
    }
}
} // namespace ts