#include "Renderer.hpp"

namespace VulkanBase
{
	Renderer::Renderer(Window& window, Device& device)
		: window(window), device(device)

	{
		swapChain = std::make_unique<SwapChain>(device, window.getExtent());
		commandBuffers.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocInfo.level						  = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool				  = device.getCommandPool();
		allocInfo.commandBufferCount		  = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate command buffers!");
	}

	Renderer::~Renderer() { vkFreeCommandBuffers(device.device(), device.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data()); }

	std::unique_ptr<SwapChain> Renderer::recreateSwapChain()
	{
		std::unique_ptr<SwapChain> oldSwapChain = std::move(swapChain);
		swapChain								= std::make_unique<VulkanBase::SwapChain>(device, window.getExtent(), oldSwapChain.get());
		return oldSwapChain;
	}

	VkCommandBuffer Renderer::beginCommandBuffer()
	{
		auto res = swapChain->acquireNextImage(&imageIndex);

		if (res == VK_ERROR_OUT_OF_DATE_KHR) return nullptr;

		else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR)
			throw std::runtime_error("Failed to acquire swapchain image!");

		VkCommandBufferBeginInfo info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };

		if (vkBeginCommandBuffer(commandBuffers[frameIndex], &info))
			throw std::runtime_error("Failed to begin recording command buffer!");

		return commandBuffers[frameIndex];
	}

	void Renderer::endCommandBuffer()
	{
		if (vkEndCommandBuffer(commandBuffers[frameIndex]) != VK_SUCCESS)
			throw std::runtime_error("Failed to record command buffer!");

		auto res = swapChain->submitCommandBuffers(&commandBuffers[frameIndex], &imageIndex);

		if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
			recreateSwapChain();

		if (res != VK_SUCCESS) throw std::runtime_error("Failed to present swapchain image!");

		frameIndex = (frameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
	}

	void Renderer::beginRenderPass(VkCommandBuffer commandBuffer)
	{
		VkRenderPassBeginInfo renderPassInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		renderPassInfo.renderPass			 = swapChain->getRenderPass();
		renderPassInfo.framebuffer			 = swapChain->getFrameBuffer(imageIndex);
		renderPassInfo.renderArea.offset	 = { 0, 0 };
		renderPassInfo.renderArea.extent	 = swapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color					= { { 0.0f, 0.0f, 0.0f, 1.0f } };
		clearValues[1].depthStencil				= { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues	   = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[frameIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport vp = {};
		vp.x		  = 0.f;
		vp.y		  = 0.f;
		vp.width	  = static_cast<float>(swapChain->getSwapChainExtent().width);
		vp.height	  = static_cast<float>(swapChain->getSwapChainExtent().height);
		vp.minDepth	  = 0.f;
		vp.maxDepth	  = 1.f;

		VkRect2D scissor = {
			{ 0, 0 },
			swapChain->getSwapChainExtent()
		};

		vkCmdSetViewport(commandBuffers[frameIndex], 0, 1, &vp);
		vkCmdSetScissor(commandBuffers[frameIndex], 0, 1, &scissor);
	}

	void Renderer::endRenderPass(VkCommandBuffer commandBuffer)
	{
		vkCmdEndRenderPass(commandBuffers[frameIndex]);
	}

	VkRenderPass Renderer::getRenderPass()
	{
		return swapChain->getRenderPass();
	}

	bool Renderer::isCompatible(SwapChain const& oldSwapChain)
	{
		return swapChain->compareFormats(oldSwapChain);
	}
}