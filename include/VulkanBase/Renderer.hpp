#include "Device.hpp"
#include "SwapChain.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <vector>

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
			float					   getAspectRatio() const { return swapChain->extentAspectRatio(); }

			int getFrameIndex() const { return frameIndex; }
	};
}