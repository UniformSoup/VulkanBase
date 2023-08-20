#pragma once
#include "Device.hpp"

#include <string>
#include <vulkan/vulkan.hpp>

namespace VulkanBase
{
	struct PipelineConfig
	{
			static void defaultConfig(PipelineConfig& configInfo);

			PipelineConfig(PipelineConfig&)					 = delete;
			PipelineConfig& operator=(PipelineConfig const&) = delete;

			std::vector<VkDynamicState>			   dynamicStateEnables;
			VkPipelineDynamicStateCreateInfo	   dynamicStateInfo;
			VkPipelineViewportStateCreateInfo	   viewportInfo			= {};
			VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo	= {};
			VkPipelineRasterizationStateCreateInfo rasterizationInfo	= {};
			VkPipelineMultisampleStateCreateInfo   multisampleInfo		= {};
			VkPipelineColorBlendAttachmentState	   colorBlendAttachment = {};
			VkPipelineColorBlendStateCreateInfo	   colorBlendInfo		= {};
			VkPipelineDepthStencilStateCreateInfo  depthStencilInfo		= {};
			VkPipelineLayout					   pipelineLayout		= VK_NULL_HANDLE;
			VkRenderPass						   renderPass			= VK_NULL_HANDLE;
			uint32_t							   subpass				= 0;
	};

	class Pipeline
	{
			Device&		   device;
			VkPipeline	   graphicsPipeline;
			VkShaderModule vertexShaderModule, fragmentShaderModule;

		public:

			Pipeline(Device& device, std::string const& vertexPath, std::string const& fragmentPath, std::vector<VkVertexInputBindingDescription> const& bindingDescs, std::vector<VkVertexInputAttributeDescription> const& attribDescs, PipelineConfig const& config);
			~Pipeline();

			void bind(VkCommandBuffer const& cmdBuf) const;

			Pipeline(Pipeline const&)			 = delete;
			Pipeline& operator=(Pipeline const&) = delete;
	};
}