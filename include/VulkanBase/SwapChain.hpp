#pragma once

#include "Device.hpp"

#include <memory>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace VulkanBase
{

    class SwapChain
    {
      public:

        static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

        SwapChain(Device& deviceRef, VkExtent2D windowExtent, std::unique_ptr<SwapChain> const&& old = nullptr);
        ~SwapChain();

        SwapChain(SwapChain const&)            = delete;
        SwapChain& operator=(SwapChain const&) = delete;

        VkFramebuffer getFrameBuffer(int index) { return swapChainFramebuffers[index]; }
        VkRenderPass  getRenderPass() { return renderPass; }
        VkImageView   getImageView(int index) { return swapChainImageViews[index]; }
        size_t        imageCount() { return swapChainImages.size(); }
        VkFormat      getSwapChainImageFormat() { return swapChainImageFormat; }
        VkExtent2D    getSwapChainExtent() { return swapChainExtent; }
        uint32_t      width() { return swapChainExtent.width; }
        uint32_t      height() { return swapChainExtent.height; }

        float extentAspectRatio()
        {
            return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
        }
        VkFormat findDepthFormat();

        VkResult acquireNextImage(uint32_t* imageIndex);
        VkResult submitCommandBuffers(VkCommandBuffer const* buffers, uint32_t* imageIndex);

      private:

        void createSwapChain(VkSwapchainKHR const& old);
        void createImageViews();
        void createDepthResources();
        void createRenderPass();
        void createFramebuffers();
        void createSyncObjects();

        // Helper functions
        VkSurfaceFormatKHR chooseSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> const& availableFormats);
        VkPresentModeKHR   chooseSwapPresentMode(std::vector<VkPresentModeKHR> const& availablePresentModes);
        VkExtent2D         chooseSwapExtent(VkSurfaceCapabilitiesKHR const& capabilities);

        VkFormat   swapChainImageFormat;
        VkExtent2D swapChainExtent;

        std::vector<VkFramebuffer> swapChainFramebuffers;
        VkRenderPass               renderPass;

        std::vector<VkImage>        depthImages;
        std::vector<VkDeviceMemory> depthImageMemorys;
        std::vector<VkImageView>    depthImageViews;
        std::vector<VkImage>        swapChainImages;
        std::vector<VkImageView>    swapChainImageViews;

        Device&    device;
        VkExtent2D windowExtent;

        VkSwapchainKHR swapChain;

        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence>     inFlightFences;
        std::vector<VkFence>     imagesInFlight;
        size_t                   currentFrame = 0;
    };

} // namespace VulkanBase
