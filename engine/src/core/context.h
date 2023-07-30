#pragma once

#include "utils.hpp"

#include "vulkan/vulkan.h"
#include "openxr/openxr.h"

namespace ts
{
class Context final
{
    NOT_COPYABLE_AND_MOVEABLE(Context);

    static constexpr XrEnvironmentBlendMode xrEnvironmentBlendMode{XR_ENVIRONMENT_BLEND_MODE_OPAQUE};

public:
    Context() = default;
    ~Context();

    static constexpr XrViewConfigurationType xrViewType{XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO};

    void createOpenXrContext();
    void createVulkanContext();
    void createVkDevice(VkSurfaceKHR vkMirrorSurface);
    void sync() const;

    XrInstance getXrInstance() const { return mXrInstance; }
    VkInstance getVkInstance() const { return mVkInstance; }
    VkPhysicalDevice getVkPhysicalDevice() const { return mPhysicalDevice; }
    VkDevice getVkDevice() const { return mVkDevice; }
    VkSampleCountFlagBits getVkMultisampleCount() const { return mVkMultisampleCount; }
    uint32_t getVkGraphicsQueueFamilyIndex() const;
    uint32_t getVkPresentQueueFamilyIndex() const;
    XrSystemId getXrSystemId() const { return mXrSystemId; }
    VkQueue getVkGraphicsQueue() const { return mVkGraphicsQueue; }
    VkQueue getVkPresentQueue() const { return mVkPresentQueue; }
    VkDeviceSize getUniformBufferOffsetAlignment() const { return mVkUniformBufferOffsetAlignment; }

private:
#ifndef NDEBUG
    void createXrDebugMessenger();
    void createVkDebugMessenger();

    XrDebugUtilsMessengerEXT mXrDebugMessenger{};
    VkDebugUtilsMessengerEXT mVkDebugMessenger{};
    static constexpr std::array vkLayers{"VK_LAYER_KHRONOS_validation"};
#endif // DEBUG

    void createXrInstance();
    void loadXrExtensions();
    void initXrSystemId();
    void getXrSystemInfo();
    void isXrBlendModeAvailable();

    void getRequiredVulkanInstanceExtensions(std::vector<std::string>& requiredVulkanInstanceExtensions);
    void isVulkanInstanceExtensionsAvailable(const std::vector<std::string>& requiredVulkanInstanceExtensions);
    void getSupportedVulkanInstanceExtensions(std::vector<VkExtensionProperties>& supportedVulkanInstanceExtensions);
    void createVulkanInstance(const std::vector<std::string>& vulkanInstanceExtensions);
    void createPhysicalDevice();
    void getGraphicsQueue();
    void getPresentQueue(const VkSurfaceKHR mirrorSurface);
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

    XrInstance mXrInstance{};
    XrSystemId mXrSystemId{};

    VkInstance mVkInstance{};
    VkPhysicalDevice mPhysicalDevice{};
    std::optional<uint32_t> mVkGraphicsQueueFamilyIndex, mVkPresentQueueFamilyIndex;
    VkDevice mVkDevice{};
    VkQueue mVkGraphicsQueue{}, mVkPresentQueue{};
    VkSampleCountFlagBits mVkMultisampleCount{};
    VkDeviceSize mVkUniformBufferOffsetAlignment{};
    std::string mXrDeviceName;
};
} // namespace ts