#pragma once

#include "internal_utils.h"

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
    MirrorView(const Context& ctx, const std::shared_ptr<Window> window);
    ~MirrorView();

    enum class RenderResult
    {
        VISIBLE,
        INVISIBLE
    };

    void createSurface();
    void connect(const Headset* headset, const Renderer* renderer);
    MirrorView::RenderResult render(uint32_t swapchainImageIndex);
    void present();

    void onWindowResize() { mIsResizeDetected = true; }

    [[nodiscard]] VkSurfaceKHR getSurface() const { return mSurface; }

private:
    void recreateXrSwapchain();
    void getSurfaceCapabilitiesAndExtent();
    bool isWindowMinimize();
    void pickSurfaceFormat();
    void createSwapchain();

    const Context& mCtx;
    const std::weak_ptr<Window> mWindow;
    const Headset* mHeadset;
    VkSurfaceKHR mSurface{};
    const Renderer* mRenderer;
    VkSurfaceCapabilitiesKHR mSurfaceCapabilities{};
    VkSwapchainKHR mSwapchain{};
    VkExtent2D mSwapchainResolution{};
    std::vector<VkImage> mSwapchainImages;
    VkSurfaceFormatKHR mSurfaceFormat{};
    bool mIsResizeDetected{};
    uint32_t mDestinationImageIndex{};
};
}