#include "Device.hpp"
#include "SwapChain.hpp"

#include <memory>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace VulkanBase
{
	class Renderer
	{
			Window&						 window;
			Device&						 device;
			std::unique_ptr<SwapChain>	 swapChain;
			std::vector<VkCommandBuffer> commandBuffers;

			uint32_t imageIndex;
			int		 frameIndex;

		public:

			Renderer(Window& window, Device& device);
			~Renderer();
			Renderer(Renderer const&)			 = delete;
			Renderer& operator=(Renderer const&) = delete;

			VkCommandBuffer beginCommandBuffer();
			void			endCommandBuffer();

			void					   beginRenderPass(VkCommandBuffer commandBuffer);
			void					   endRenderPass(VkCommandBuffer commandBuffer);
			std::unique_ptr<SwapChain> recreateSwapChain();
			bool					   isCompatible(SwapChain const& oldSwapChain);
			VkRenderPass			   getRenderPass();

			int getFrameIndex() const { return frameIndex; }
	};
}