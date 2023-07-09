#pragma once

#include "Window.hpp"

#include <string>
#include <vector>
#include <vulkan/vulkan.hpp>

namespace VulkanBase
{

	struct SwapChainSupportDetails
	{
			VkSurfaceCapabilitiesKHR		capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR>	presentModes;
	};

	struct QueueFamilyIndices
	{
			uint32_t graphicsFamily;
			uint32_t presentFamily;
			bool	 graphicsFamilyHasValue = false;
			bool	 presentFamilyHasValue	= false;

			bool isComplete() { return graphicsFamilyHasValue && presentFamilyHasValue; }
	};

	class Device
	{
		public:

			Device(Window& window);
			~Device();

			Device(Device const&)			 = delete;
			Device& operator=(Device const&) = delete;
			Device(Device&&)				 = delete;
			Device& operator=(Device&&)		 = delete;

			VkCommandPool getCommandPool() { return commandPool; }

			VkDevice device() { return device_; }

			VkSurfaceKHR surface() { return surface_; }

			VkQueue graphicsQueue() { return graphicsQueue_; }

			VkQueue presentQueue() { return presentQueue_; }

			SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(physicalDevice); }

			uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

			QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(physicalDevice); }

			VkFormat		findSupportedFormat(std::vector<VkFormat> const& candidates, VkImageTiling tiling,
												VkFormatFeatureFlags features);
			void			createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
										 VkBuffer& buffer, VkDeviceMemory& bufferMemory);
			VkCommandBuffer beginSingleTimeCommands();
			void			endSingleTimeCommands(VkCommandBuffer commandBuffer);
			void			copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
			void			copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

			void createImageWithInfo(VkImageCreateInfo const& imageInfo, VkMemoryPropertyFlags properties, VkImage& image,
									 VkDeviceMemory& imageMemory);

			VkPhysicalDeviceProperties properties;

		private:

			void createInstance();
			void pickPhysicalDevice();
			void createLogicalDevice();
			void createCommandPool();

			bool					 isDeviceSuitable(VkPhysicalDevice device);
			std::vector<char const*> getRequiredExtensions();
			bool					 areValidationLayersSupported();
			QueueFamilyIndices		 findQueueFamilies(VkPhysicalDevice device);
			bool					 hasRequiredInstanceExtensions();
			bool					 checkDeviceExtensionSupport(VkPhysicalDevice device);
			SwapChainSupportDetails	 querySwapChainSupport(VkPhysicalDevice device);

			VkInstance instance;
#ifdef DEBUG
			VkDebugUtilsMessengerEXT debugMessenger;
#endif
			VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
			Window&			 window;
			VkCommandPool	 commandPool;

			VkDevice	 device_;
			VkSurfaceKHR surface_;
			VkQueue		 graphicsQueue_;
			VkQueue		 presentQueue_;

			const std::vector<char const*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
			const std::vector<char const*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#ifdef __APPLE__
															   "VK_KHR_portability_subset"
#endif
			};
	};
}	 // namespace VulkanBase