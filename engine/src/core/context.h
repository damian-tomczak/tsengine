#pragma once

#include "utils.hpp"

#include "vulkan/shaders_compiler.h"
#include "vulkan/vulkan_loader.h"
#include "openxr/openxr.h"
#include "tsengine/logger.h"
#include "os.h"

namespace ts
{
class Context final
{
    NOT_COPYABLE_AND_MOVEABLE(Context);

public:
    Context() = default;
    ~Context();

    static constexpr XrViewConfigurationType xrViewType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
    static constexpr XrEnvironmentBlendMode xrEnvironmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;

    void createOpenXrContext();
    void createVulkanContext();
    void createVkDevice(VkSurfaceKHR pMirrorSurface);

    XrInstance getXrInstance() const { return mpXrInstance; }
    VkInstance getVkInstance() const { return mpVkInstance; }
    VkPhysicalDevice getVkPhysicalDevice() const { return mpPhysicalDevice; }
    VkDevice getVkDevice() const { return mpVkDevice; }
    VkSampleCountFlagBits getMultisampleCount() const { return mMultisampleCount; }
    uint32_t getGraphicsQueueFamilyIndex() const { return *mGraphicsQueueFamilyIndex; };
    uint32_t getPresentQueueFamilyIndex() const { return *mPresentQueueFamilyIndex; };
    XrSystemId getXrSystemId() const { return mpXrSystemId;  }

private:
#ifndef NDEBUG
    void createXrDebugMessenger();
    void createVkDebugMessenger();

    XrDebugUtilsMessengerEXT mXrDebugMessenger{};
    static constexpr std::array vkLayers = {"VK_LAYER_KHRONOS_validation"};

    VkDebugUtilsMessengerEXT mVkDebugMessenger{};
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
    void getPresentQueue(const VkSurfaceKHR pMirrorSurface);
    void isVulkanDeviceExtensionsAvailable(
        std::vector<std::string>& requiredVulkanDeviceExtensions,
        VkPhysicalDeviceFeatures& physicalDeviceFeatures,
        VkPhysicalDeviceMultiviewFeatures& physicalDeviceMultiviewFeatures,
        VkPhysicalDeviceFeatures2& physicalDeviceFeatures2);
    void getSupportedVulkanDeviceExtensions(std::vector<VkExtensionProperties>& vulkanDeviceExtensions);
    void getRequiredVulkanDeviceExtensions(std::vector<std::string>& vulkanDeviceExtensions);
    void createLogicalDevice(
        const std::vector<std::string>& requiredVulkanDeviceExtensions,
        const VkPhysicalDeviceFeatures& physicalDeviceFeatures,
        const VkPhysicalDeviceMultiviewFeatures& physicalDeviceMultiviewFeatures,
        std::vector<VkDeviceQueueCreateInfo>& deviceQueueCis);
    void createQueues(std::vector<VkDeviceQueueCreateInfo>& deviceQueueCis);

    XrInstance mpXrInstance{};
    XrSystemId mpXrSystemId{};

    VkInstance mpVkInstance{};
    VkPhysicalDevice mpPhysicalDevice{};
    std::optional<uint32_t> mGraphicsQueueFamilyIndex, mPresentQueueFamilyIndex;
    VkDevice mpVkDevice{};
    VkQueue mpVkGraphicsQueue{}, mpPresentQueue{};
    VkSampleCountFlagBits mMultisampleCount{};
    VkDeviceSize mUniformBufferOffsetAlignment{};
};
} // namespace ts