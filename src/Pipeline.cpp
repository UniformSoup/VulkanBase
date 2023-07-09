#include "Pipeline.hpp"

#include "Model.hpp"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

std::vector<uint8_t> read(std::string const& filepath)
{
	std::ifstream file(filepath, std::ios::binary);
	if (!file.is_open()) throw std::runtime_error("Failed to open: " + filepath);

	return std::vector<uint8_t>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

void createShaderModule(VkDevice device, std::vector<uint8_t> const& code, VkShaderModule* const pShaderModule)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType					= VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize					= code.size();
	createInfo.pCode					= reinterpret_cast<uint32_t const*>(code.data());

	if (vkCreateShaderModule(device, &createInfo, nullptr, pShaderModule) != VK_SUCCESS)
		throw std::runtime_error("Failed to create shader module!");
}

namespace VulkanBase
{
	Pipeline::Pipeline(Device& device, std::string const& vertexPath, std::string const& fragmentPath,
					   PipelineConfig const& config)
		: device(device)
	{
		std::vector<uint8_t> vertex	  = read(vertexPath);
		std::vector<uint8_t> fragment = read(fragmentPath);

		createShaderModule(device.device(), vertex, &vertexShaderModule);
		createShaderModule(device.device(), fragment, &fragmentShaderModule);

		VkPipelineShaderStageCreateInfo shaderStages[2] = {};	 // Holy shit i cant believe the memory wasnt zeroed.

		shaderStages[0].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[0].stage  = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStages[0].module = vertexShaderModule;
		shaderStages[0].pName  = "main";
		shaderStages[0].flags  = 0;
		shaderStages[0].pNext  = nullptr;

		shaderStages[1].sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[1].stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStages[1].module = fragmentShaderModule;
		shaderStages[1].pName  = "main";
		shaderStages[1].flags  = 0;
		shaderStages[1].pNext  = nullptr;

		auto bindingDescs = Vertex::getBindingDescriptions();
		auto attribDescs  = Vertex::getAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType								 = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount		 = static_cast<uint32_t>(attribDescs.size());
		vertexInputInfo.vertexBindingDescriptionCount		 = static_cast<uint32_t>(bindingDescs.size());
		;
		vertexInputInfo.pVertexAttributeDescriptions = attribDescs.data();
		vertexInputInfo.pVertexBindingDescriptions	 = bindingDescs.data();

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType						  = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount					  = 2;
		pipelineInfo.pStages					  = shaderStages;
		pipelineInfo.pVertexInputState			  = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState		  = &config.inputAssemblyInfo;
		pipelineInfo.pViewportState				  = &config.viewportInfo;
		pipelineInfo.pRasterizationState		  = &config.rasterizationInfo;
		pipelineInfo.pMultisampleState			  = &config.multisampleInfo;
		pipelineInfo.pColorBlendState			  = &config.colorBlendInfo;
		pipelineInfo.pDepthStencilState			  = &config.depthStencilInfo;
		pipelineInfo.pDynamicState				  = &config.dynamicStateInfo;

		pipelineInfo.layout		= config.pipelineLayout;
		pipelineInfo.renderPass = config.renderPass;
		pipelineInfo.subpass	= config.subpass;

		pipelineInfo.basePipelineIndex	= -1;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(device.device(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
			throw std::runtime_error("Failed to create graphics pipeline!");
	}

	Pipeline::~Pipeline()
	{
		vkDestroyShaderModule(device.device(), vertexShaderModule, nullptr);
		vkDestroyShaderModule(device.device(), fragmentShaderModule, nullptr);
		vkDestroyPipeline(device.device(), graphicsPipeline, nullptr);
	}

	void Pipeline::bind(VkCommandBuffer const& cmdBuf) const
	{
		vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	}

	void PipelineConfig::defaultConfig(PipelineConfig& config)
	{
		config.inputAssemblyInfo.sType					= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		config.inputAssemblyInfo.topology				= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		config.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		config.viewportInfo.sType		  = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		config.viewportInfo.viewportCount = 1;
		config.viewportInfo.pViewports	  = nullptr;
		config.viewportInfo.scissorCount  = 1;
		config.viewportInfo.pScissors	  = nullptr;

		config.rasterizationInfo.sType					 = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		config.rasterizationInfo.depthClampEnable		 = VK_FALSE;
		config.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		config.rasterizationInfo.polygonMode			 = VK_POLYGON_MODE_FILL;
		config.rasterizationInfo.lineWidth				 = 1.0f;
		config.rasterizationInfo.cullMode				 = VK_CULL_MODE_BACK_BIT;	 // SET TO VK_CULL_MODE_BACK when ready
		config.rasterizationInfo.frontFace				 = VK_FRONT_FACE_CLOCKWISE;
		config.rasterizationInfo.depthBiasEnable		 = VK_FALSE;
		config.rasterizationInfo.depthBiasConstantFactor = 0.0f;	// Optional
		config.rasterizationInfo.depthBiasClamp			 = 0.0f;	// Optional
		config.rasterizationInfo.depthBiasSlopeFactor	 = 0.0f;	// Optional

		config.multisampleInfo.sType				 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		config.multisampleInfo.sampleShadingEnable	 = VK_FALSE;
		config.multisampleInfo.rasterizationSamples	 = VK_SAMPLE_COUNT_1_BIT;
		config.multisampleInfo.minSampleShading		 = 1.0f;		// Optional
		config.multisampleInfo.pSampleMask			 = nullptr;		// Optional
		config.multisampleInfo.alphaToCoverageEnable = VK_FALSE;	// Optional
		config.multisampleInfo.alphaToOneEnable		 = VK_FALSE;	// Optional

		config.colorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		config.colorBlendAttachment.blendEnable			= VK_FALSE;
		config.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;	   // Optional
		config.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;	   // Optional
		config.colorBlendAttachment.colorBlendOp		= VK_BLEND_OP_ADD;		   // Optional
		config.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;	   // Optional
		config.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;	   // Optional
		config.colorBlendAttachment.alphaBlendOp		= VK_BLEND_OP_ADD;		   // Optional

		config.colorBlendInfo.sType				= VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		config.colorBlendInfo.logicOpEnable		= VK_FALSE;
		config.colorBlendInfo.logicOp			= VK_LOGIC_OP_COPY;	   // Optional
		config.colorBlendInfo.attachmentCount	= 1;
		config.colorBlendInfo.pAttachments		= &config.colorBlendAttachment;
		config.colorBlendInfo.blendConstants[0] = 0.0f;	   // Optional
		config.colorBlendInfo.blendConstants[1] = 0.0f;	   // Optional
		config.colorBlendInfo.blendConstants[2] = 0.0f;	   // Optional
		config.colorBlendInfo.blendConstants[3] = 0.0f;	   // Optional

		config.depthStencilInfo.sType				  = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		config.depthStencilInfo.depthTestEnable		  = VK_TRUE;
		config.depthStencilInfo.depthWriteEnable	  = VK_TRUE;
		config.depthStencilInfo.depthCompareOp		  = VK_COMPARE_OP_LESS;
		config.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		config.depthStencilInfo.minDepthBounds		  = 0.0f;	 // Optional
		config.depthStencilInfo.maxDepthBounds		  = 1.0f;	 // Optional
		config.depthStencilInfo.stencilTestEnable	  = VK_FALSE;
		config.depthStencilInfo.front				  = {};	   // Optional
		config.depthStencilInfo.back				  = {};	   // Optional

		config.dynamicStateEnables				  = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		config.dynamicStateInfo.sType			  = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		config.dynamicStateInfo.pDynamicStates	  = config.dynamicStateEnables.data();
		config.dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(config.dynamicStateEnables.size());
		config.dynamicStateInfo.flags			  = 0;
	}
}	 // namespace VulkanBase