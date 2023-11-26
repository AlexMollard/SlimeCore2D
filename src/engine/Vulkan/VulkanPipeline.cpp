#include "VulkanPipeline.h"
#include "VulkanStructs.h"
#include "VulkanShaderModule.h"
#include "engine/ConsoleLog.h"
#include <engine/ResourceManager.h>

VulkanPipeline::VulkanPipeline(VulkanDevice& device, VulkanRenderPass& renderPass, vk::SurfaceKHR surface) : m_device(device), m_renderPass(renderPass)
{
	vk::DescriptorSetLayout descriptorSetLayout = CreateDescriptorSetLayout();

	// Create the pipeline layout
	vk::PipelineLayoutCreateInfo layoutInfo;
	layoutInfo.setLayoutCount = 1;
	layoutInfo.pSetLayouts    = &descriptorSetLayout;
	
	vk::PipelineLayout pipelineLayout;
	if (m_device.GetLogicalDevice().createPipelineLayout(&layoutInfo, nullptr, &pipelineLayout) != vk::Result::eSuccess)
	{
		SLIME_ERROR("Failed to create pipeline layout!");
	}

	// Shader stages
	std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
	const std::string& vertShader = ResourceManager::GetVulkanShaderPath("Basic", static_cast<int>(vk::ShaderStageFlagBits::eVertex));
	const std::string& fragShader = ResourceManager::GetVulkanShaderPath("Basic", static_cast<int>(vk::ShaderStageFlagBits::eFragment));

	vertShaderModule = new VulkanShaderModule(device.GetLogicalDevice(), vertShader);
	fragShaderModule = new VulkanShaderModule(device.GetLogicalDevice(), fragShader);

	vk::PipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.stage                             = vk::ShaderStageFlagBits::eVertex;
	vertShaderStageInfo.module                            = vertShaderModule->GetShaderModule();
	vertShaderStageInfo.pName                             = "main";
	shaderStages.push_back(vertShaderStageInfo);

	vk::PipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.stage                             = vk::ShaderStageFlagBits::eFragment;
	fragShaderStageInfo.module                            = fragShaderModule->GetShaderModule();
	fragShaderStageInfo.pName                             = "main";
	shaderStages.push_back(fragShaderStageInfo);

	// Vertex input
	auto bindingDescription    = VulkanStructs::Vertex::GetBindingDescription();
	auto attributeDescriptions = VulkanStructs::Vertex::GetAttributeDescriptions();

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.vertexBindingDescriptionCount          = 1;
	vertexInputInfo.pVertexBindingDescriptions             = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount        = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions           = attributeDescriptions.data();

	// Depth and stencil testing
	vk::PipelineDepthStencilStateCreateInfo depthStencil = {};
	depthStencil.depthTestEnable                         = VK_TRUE;
	depthStencil.depthWriteEnable                        = VK_TRUE;
	depthStencil.depthCompareOp                          = vk::CompareOp::eLess;
	depthStencil.depthBoundsTestEnable                   = VK_FALSE;
	depthStencil.stencilTestEnable                       = VK_FALSE;

	// Color blending
	vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
	colorBlendAttachment.blendEnable    = VK_FALSE;

	vk::PipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.logicOpEnable                         = VK_FALSE;
	colorBlending.attachmentCount                       = 1;
	colorBlending.pAttachments                          = &colorBlendAttachment;

	// Dynamic states
	std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };

	vk::PipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.dynamicStateCount                  = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates                     = dynamicStates.data();

	// Viewport and scissor
	vk::Viewport viewport = {};
	vk::Rect2D scissor    = {};
	SetupViewportAndScissor(surface, viewport, scissor);

	// Input assembly
	vk::PipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.topology                                 = vk::PrimitiveTopology::eTriangleList;
	inputAssembly.primitiveRestartEnable                   = VK_FALSE;

	// Viewport state
	vk::PipelineViewportStateCreateInfo viewportState = {};
	viewportState.viewportCount                       = 1;
	viewportState.pViewports                          = &viewport;
	viewportState.scissorCount                        = 1;
	viewportState.pScissors                           = &scissor;

	// Rasterizer
	vk::PipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.depthClampEnable                         = VK_FALSE;
	rasterizer.rasterizerDiscardEnable                  = VK_FALSE;
	rasterizer.polygonMode                              = vk::PolygonMode::eFill;
	rasterizer.lineWidth                                = 1.0f;
	rasterizer.cullMode                                 = vk::CullModeFlagBits::eBack;
	rasterizer.frontFace                                = vk::FrontFace::eClockwise;
	rasterizer.depthBiasEnable                          = VK_FALSE;

	// Multisampling
	vk::PipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sampleShadingEnable                    = VK_FALSE;
	multisampling.rasterizationSamples                   = vk::SampleCountFlagBits::e1;

	// Pipeline
	vk::GraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.stageCount                     = static_cast<uint32_t>(shaderStages.size());
	pipelineInfo.pStages                        = shaderStages.data();
	pipelineInfo.pVertexInputState              = &vertexInputInfo;

	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState      = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState   = &multisampling;

	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState   = &colorBlending;
	pipelineInfo.pDynamicState      = &dynamicState;
	pipelineInfo.layout             = pipelineLayout;

	pipelineInfo.renderPass = renderPass.GetRenderPass();

	pipelineInfo.subpass            = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	m_graphicsPipeline = m_device.GetLogicalDevice().createGraphicsPipeline(VK_NULL_HANDLE, pipelineInfo).value;
}

VulkanPipeline::~VulkanPipeline() 
{
	if (vertShaderModule)
	{
		delete vertShaderModule;
		vertShaderModule = nullptr;
	}

	if (fragShaderModule)
	{
		delete fragShaderModule;
		fragShaderModule = nullptr;
	}

	if (m_graphicsPipeline)
	{
		m_device.GetLogicalDevice().destroyPipeline(m_graphicsPipeline);
		m_graphicsPipeline = nullptr;
	}
}

void VulkanPipeline::SetupViewportAndScissor(vk::SurfaceKHR surface, vk::Viewport& viewport, vk::Rect2D& scissor)
{
	vk::SurfaceCapabilitiesKHR surfaceCapabilities = m_device.GetPhysicalDevice().getSurfaceCapabilitiesKHR(surface);

	viewport.x        = 0.0f;
	viewport.y        = 0.0f;
	viewport.width    = static_cast<float>(surfaceCapabilities.currentExtent.width);
	viewport.height   = static_cast<float>(surfaceCapabilities.currentExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	scissor.offset = vk::Offset2D(0, 0);
	scissor.extent = surfaceCapabilities.currentExtent;
}

vk::DescriptorSetLayout VulkanPipeline::CreateDescriptorSetLayout()
{
	// Descriptor set layout bindings for uniform buffers and texture samplers
	std::vector<vk::DescriptorSetLayoutBinding> bindings = { // Binding 0: Combined image sampler for textureSampler
		                                                     { 0, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr }
	};

	// Descriptor set layout create info
	vk::DescriptorSetLayoutCreateInfo layoutInfo;
	layoutInfo.setBindings(bindings);

	// Create the descriptor set layout
	return m_device.GetLogicalDevice().createDescriptorSetLayout(layoutInfo);
}
