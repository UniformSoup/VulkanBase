#include "Renderer.hpp"

namespace VulkanBase
{
	Renderer::Renderer(Window& window, Device& device)
		: window(window), device(device)

	{
		swapChain = std::make_unique<VulkanBase::SwapChain>(device, window.getExtent());
		commandBuffers.resize(swapChain->imageCount());

		VkCommandBufferAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocInfo.level						  = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool				  = device.getCommandPool();
		allocInfo.commandBufferCount		  = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate command buffers!");
	}

	Renderer::~Renderer() { vkFreeCommandBuffers(device.device(), device.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data()); }

	void Renderer::recreateSwapChain()
	{
        std::unique_ptr<SwapChain> oldSwapChain = std::move(swapChain);
		swapChain = std::make_unique<VulkanBase::SwapChain>(device, window.getExtent(), oldSwapChain.get());

		if (swapChain->imageCount() != commandBuffers.size())
		{
			vkFreeCommandBuffers(device.device(), device.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()),
								 commandBuffers.data());
			commandBuffers.clear();
			commandBuffers.resize(swapChain->imageCount());

			VkCommandBufferAllocateInfo allocInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
			allocInfo.level						  = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool				  = device.getCommandPool();
			allocInfo.commandBufferCount		  = static_cast<uint32_t>(commandBuffers.size());

			if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
				throw std::runtime_error("Failed to allocate command buffers!");
		}

        if (!swapChain->compareFormats(*(oldSwapChain.get())))
            throw std::runtime_error("The swapchain image format has changed!");
	}

	VkCommandBuffer Renderer::beginCommandBuffer()
	{
		auto res = swapChain->acquireNextImage(&currentImage);

		if (res == VK_ERROR_OUT_OF_DATE_KHR) return nullptr;

		else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR)
			throw std::runtime_error("Failed to acquire swapchain image!");

		VkCommandBufferBeginInfo info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };

		if (vkBeginCommandBuffer(commandBuffers[currentImage], &info))
			throw std::runtime_error("Failed to begin recording command buffer!");

		return commandBuffers[currentImage];
	}

	void Renderer::endCommandBuffer()
	{
		if (vkEndCommandBuffer(commandBuffers[currentImage]) != VK_SUCCESS)
			throw std::runtime_error("Failed to record command buffer!");

		auto res = swapChain->submitCommandBuffers(&commandBuffers[currentImage], &currentImage);

		if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
			recreateSwapChain();

		if (res != VK_SUCCESS) throw std::runtime_error("Failed to present swapchain image!");
	}

	void Renderer::beginRenderPass(VkCommandBuffer commandBuffer)
	{
		VkRenderPassBeginInfo renderPassInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
		renderPassInfo.renderPass			 = swapChain->getRenderPass();
		renderPassInfo.framebuffer			 = swapChain->getFrameBuffer(currentImage);
		renderPassInfo.renderArea.offset	 = { 0, 0 };
		renderPassInfo.renderArea.extent	 = swapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color					= { { 0.0f, 0.0f, 0.0f, 1.0f } };
		clearValues[1].depthStencil				= { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues	   = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[currentImage], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

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

		vkCmdSetViewport(commandBuffers[currentImage], 0, 1, &vp);
		vkCmdSetScissor(commandBuffers[currentImage], 0, 1, &scissor);
	}

	void Renderer::endRenderPass(VkCommandBuffer commandBuffer)
	{
		vkCmdEndRenderPass(commandBuffers[currentImage]);
	}

	VkRenderPass Renderer::getRenderPass()
	{
		return swapChain->getRenderPass();
	}
}