#pragma once

#include "internal_utils.h"

#include "vulkan/vulkan.h"
#include "openxr/openxr.h"

namespace ts
{
class Context final
{
    NOT_COPYABLE_AND_MOVEABLE(Context);

    static constexpr XrEnvironmentBlendMode xrEnvironmentBlendMode{XR_ENVIRONMENT_BLEND_MODE_OPAQUE};

public:
    Context(const std::string& gameName) : mGameName{gameName} {}
    ~Context();

    static constexpr XrViewConfigurationType xrViewType{XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO};

    Context& createOpenXrContext();
    void createVulkanContext();
    void createVkDevice(const VkSurfaceKHR vkMirrorSurface);
    void sync() const;

    [[nodiscard]] XrInstance getXrInstance() const { return mXrInstance; }
    [[nodiscard]] VkInstance getVkInstance() const { return mVkInstance; }
    [[nodiscard]] VkPhysicalDevice getVkPhysicalDevice() const { return mPhysicalDevice; }
    [[nodiscard]] VkDevice getVkDevice() const { return mVkDevice; }
    [[nodiscard]] VkSampleCountFlagBits getVkMultisampleCount() const { return mVkMultisampleCount; }
    [[nodiscard]] uint32_t getVkGraphicsQueueFamilyIndex() const;
    [[nodiscard]] uint32_t getVkPresentQueueFamilyIndex() const;
    [[nodiscard]] XrSystemId getXrSystemId() const { return mXrSystemId; }
    [[nodiscard]] VkQueue getVkGraphicsQueue() const { return mVkGraphicsQueue; }
    [[nodiscard]] VkQueue getVkPresentQueue() const { return mVkPresentQueue; }
    [[nodiscard]] VkDeviceSize getUniformBufferOffsetAlignment() const { return mVkUniformBufferOffsetAlignment; }

private:
    void createXrInstance();
    void loadXrExtensions();
    void initXrSystemId();
    void initXrSystemInfo();
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

    const std::string mGameName;

#ifndef NDEBUG
    void createXrDebugMessenger();
    void createVkDebugMessenger();

    XrDebugUtilsMessengerEXT mXrDebugMessenger{};
    VkDebugUtilsMessengerEXT mVkDebugMessenger{};
    static constexpr std::array vkLayers{ "VK_LAYER_KHRONOS_validation" };
#endif // DEBUG

    XrInstance mXrInstance{};
    XrSystemId mXrSystemId{};

    VkInstance mVkInstance{};
    VkPhysicalDevice mPhysicalDevice{};
    std::optional<uint32_t> mVkGraphicsQueueFamilyIndex, mVkPresentQueueFamilyIndex;
    VkDevice mVkDevice{};
    VkQueue mVkGraphicsQueue{}, mVkPresentQueue{};
    VkSampleCountFlagBits mVkMultisampleCount{};
    VkDeviceSize mVkUniformBufferOffsetAlignment{};
    bool mIsXrContextCreated{};
};
} // namespace ts