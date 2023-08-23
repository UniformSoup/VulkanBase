#pragma once

#include <string>
#include <vulkan/vulkan.hpp>

namespace VulkanBase
{
	class Window
	{
		public:

			Window()						 = default;
			Window(Window const&)			 = delete;
			Window& operator=(Window const&) = delete;

			virtual void					 createWindowSurface(VkInstance& instance, VkSurfaceKHR* const pSurface) = 0;
			virtual std::vector<char const*> getRequiredInstanceExtensions() const									 = 0;
			virtual VkExtent2D				 getExtent() const														 = 0;
			virtual char const*				 getName() const														 = 0;
	};
}