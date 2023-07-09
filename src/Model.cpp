#include "Model.hpp"

#include <algorithm>

namespace VulkanBase
{
	Model::Model(Device& device, std::vector<Vertex> const& vertices)
		: device(device)
	{
		vertexCount				= static_cast<uint32_t>(vertices.size());
		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
		device.createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
							VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertexBuffer,
							vertexBufferMemory);

		void* data;
		vkMapMemory(device.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
		std::copy(vertices.begin(), vertices.end(), static_cast<Vertex*>(data));
		vkUnmapMemory(device.device(), vertexBufferMemory);
	}

	Model::~Model()
	{
		vkDestroyBuffer(device.device(), vertexBuffer, nullptr);
		vkFreeMemory(device.device(), vertexBufferMemory, nullptr);
	}

	void Model::draw(VkCommandBuffer commandBuffer) const { vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0); }

	void Model::bind(VkCommandBuffer commandBuffer) const
	{
		VkBuffer	 buffers[] = {vertexBuffer};
		VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
	}

	std::vector<VkVertexInputBindingDescription> Vertex::getBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding	 = 0;
		bindingDescriptions[0].stride	 = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> Vertex::getAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attribDescriptions(2);

		attribDescriptions[0].binding  = 0;
		attribDescriptions[0].location = 0;
		attribDescriptions[0].format   = VK_FORMAT_R32G32_SFLOAT;
		attribDescriptions[0].offset   = offsetof(Vertex, pos);

		attribDescriptions[1].binding  = 0;
		attribDescriptions[1].location = 1;
		attribDescriptions[1].format   = VK_FORMAT_R32G32B32_SFLOAT;
		attribDescriptions[1].offset   = offsetof(Vertex, color);

		return attribDescriptions;
	}
}	 // namespace VulkanBase