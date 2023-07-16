#pragma once

#include "utils.hpp"

#include "vulkan/shaders_compiler.h"
#include "vulkan/vulkan_loader.h"
#include "openxr/openxr.h"
#include "tsengine/logger.h"
#include "os.h"

namespace ts
{
class Context final : public Singleton<Context>
{
    NOT_MOVEABLE(Context);

public:
    void createContext();

    VkInstance getVkInstance() { return mpVkInstance; }

private:
    friend class Singleton<Context>;
    Context() = default;
    ~Context();

#ifdef DEBUG
    void createXrDebugMessenger();
    void createVkDebugMessenger(const VkInstance instance);

    XrDebugUtilsMessengerEXT mpXrDebugMessenger{};
    static constexpr std::array vkLayers = { "VK_LAYER_KHRONOS_validation" };

    VkDebugUtilsMessengerEXT mpVkDebugMessenger{};
#endif // DEBUG

    void createXrInstance();
    void loadXrExtensions();
    void initXrSystemId();
    void checkAvailabilityXrBlendMode();
    void getRequiredVkInstanceExtensionsAndCheckAvailability(std::vector<std::string>& requiredVkInstanceExtensions);
    void createVkInstance(const std::vector<std::string>& vulkanInstanceExtensions);

    static constexpr XrViewConfigurationType xrViewType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    static constexpr XrEnvironmentBlendMode xrEnvironmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;

    XrInstance mpXrInstance{};
    XrSystemId mXrSystemId{};

    VkInstance mpVkInstance{};
};
} // namespace ts