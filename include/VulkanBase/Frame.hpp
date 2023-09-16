#include "Camera.hpp"

#include <vulkan/vulkan.h>

namespace VulkanBase
{
	struct Frame
	{
			int				index;
			float			dt;
			VkCommandBuffer cmdBuf;
			Camera&			camera;
			VkDescriptorSet descriptorSet;
	};
}