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
    SINGLETON(Context);
    NOT_MOVEABLE(Context);

public:
    void createContext();
    void createDevice(VkSurfaceKHR pMirrorSurface);

    VkInstance getVkInstance() { return mpVkInstance; }

private:
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
    void isXrBlendModeAvailable();

    void getRequiredVulkanInstanceExtensions(std::vector<std::string>& requiredVulkanInstanceExtensions);
    void isVulkanInstanceExtensionsAvailable(const std::vector<std::string>& requiredVulkanInstanceExtensions);
    void getSupportedVulkanInstanceExtensions(std::vector<VkExtensionProperties>& supportedVulkanInstanceExtensions);
    void createVulkanInstance(const std::vector<std::string>& vulkanInstanceExtensions);
    void createPhysicalDevice();
    void getGraphicsQueue();
    void getPresentQueue(VkSurfaceKHR pMirrorSurface);
    void isVulkanDeviceExtensionsAvailable();
    void getSupportedVulkanDeviceExtensions(std::vector<VkExtensionProperties>& vulkanDeviceExtensions);
    void getRequiredVulkanDeviceExtensions(std::vector<std::string>& vulkanDeviceExtensions);

    static constexpr XrViewConfigurationType xrViewType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    static constexpr XrEnvironmentBlendMode xrEnvironmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;

    XrInstance mpXrInstance{};
    XrSystemId mXrSystemId{};

    VkInstance mpVkInstance{};
    VkPhysicalDevice mpPhysicalDevice{};
    std::optional<uint32_t> mpGraphicsQueueFamilyIndex, mpPresentQueueFamilyIndex;
};
} // namespace ts