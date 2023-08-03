#pragma once

#include "Device.hpp"

#include <glm/glm.hpp>
#include <vector>

namespace VulkanBase
{
	struct Vertex
	{
			glm::vec3 pos;
			glm::vec3 color;

			static std::vector<VkVertexInputBindingDescription>	  getBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
	};

	class Model
	{
			Device&		   device;
			VkBuffer	   vertexBuffer;
			VkDeviceMemory vertexBufferMemory;
			uint32_t	   vertexCount;

			void createVertexBuffers(std::vector<Vertex> const& vertices);

		public:

			Model(Device& device, std::vector<Vertex> const& vertices);
			~Model();

			Model(Model const&)			   = delete;
			Model& operator=(Model const&) = delete;

			void bind(VkCommandBuffer commandBuffer) const;
			void draw(VkCommandBuffer commandBuffer) const;
	};
}