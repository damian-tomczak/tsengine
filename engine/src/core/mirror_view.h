#pragma once

#include "utils.hpp"

#define NOMINMAX
#include "vulkan/vulkan.h"

namespace ts
{
class Context;
class Window;
class Headset;
class Renderer;

class MirrorView final
{
    NOT_COPYABLE_AND_MOVEABLE(MirrorView);

    static constexpr VkFormat colorFormat{VK_FORMAT_B8G8R8A8_SRGB};
    static constexpr VkPresentModeKHR presentMode{VK_PRESENT_MODE_FIFO_KHR};
    static constexpr size_t mirrorEyeIndex{1};

public:
    MirrorView(const Context* ctx, const std::shared_ptr<Window> window);
    ~MirrorView();

    void connect(const Headset* headset, const Renderer* renderer);

    VkSurfaceKHR getSurface() const { return mSurface; }

private:
    void createSurface();
    void recreateSwapchain();
    void getSurfaceCapabilitiesAndExtent();
    bool isWindowMinimize();
    void pickSurfaceFormat();
    void createSwapchain();

    const Context* mCtx{};
    const std::weak_ptr<Window> mWindow;
    VkSurfaceKHR mSurface{};
    const Headset* mHeadset;
    const Renderer* mRenderer;
    VkSurfaceCapabilitiesKHR mSurfaceCapabilities{};
    VkSwapchainKHR mSwapchain{};
    VkExtent2D mSwapchainResolution{};
    std::vector<VkImage> mSwapchainImages;
    VkSurfaceFormatKHR mSurfaceFormat{};
};
}