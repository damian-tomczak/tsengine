#pragma once

#include "pch.h"
#include "tools.h"

#include "vulkan/shaders_compiler.h"
#include "vulkan/vulkan_loader.h"
#include "openxr/openxr.h"
#include "os.h"

namespace ts
{
class Context final : public Singleton<Context>
{
    NOT_MOVEABLE(Context);

public:
    void createContext(std::string_view appName);

private:
    friend class Singleton<Context>;
    Context() = default;

    void createXrInstance();
    void loadXrExtensions();
    void initXrSystemId();
    void checkAvailabilityXrBlendMode();

    static constexpr XrViewConfigurationType xrViewType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    static constexpr XrEnvironmentBlendMode xrEnvironmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;

    std::string_view mAppName;
    XrInstance mXrInstance{};

#ifdef DEBUG
    XrDebugUtilsMessengerEXT mXrDebugUtilsMessenger{};
#endif

    XrSystemId mXrSystemId{};
};
}