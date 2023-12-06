#include "Pipeline.h"

#include "VulkanEngine.h"
#include "VulkanInit.h"
#include <engine/ConsoleLog.h>
#include <engine/ResourceManager.h>
#include <fstream>
#include <vector>

VkShaderModule vkutil::LoadShaderModule(const char* filePath, VkDevice device)
{
	// open the file. With cursor at the end
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		SLIME_ERROR("Error when building the compute shader");
		return VK_NULL_HANDLE;
	}

	// find what the size of the file is by looking up the location of the cursor
	// because the cursor is at the end, it gives the size directly in bytes
	size_t fileSize = (size_t)file.tellg();

	// spirv expects the buffer to be on uint32, so make sure to reserve a int
	// vector big enough for the entire file
	std::vector<uint32_t> buffer(fileSize / sizeof(uint32_t));

	// put file cursor at beginning
	file.seekg(0);

	// load the entire file into the buffer
	file.read((char*)buffer.data(), fileSize);

	// now that the file is loaded into the buffer, we can close it
	file.close();

	// create a new shader module, using the buffer we loaded
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

	// codeSize has to be in bytes, so multply the ints in the buffer by size of
	// int to know the real size of the buffer
	createInfo.codeSize = buffer.size() * sizeof(uint32_t);
	createInfo.pCode    = buffer.data();

	// Include the extension structure for compute shader derivatives
	VkPipelineShaderStageRequiredSubgroupSizeCreateInfoEXT subgroupSizeCreateInfo = {};
	subgroupSizeCreateInfo.sType                                                  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_REQUIRED_SUBGROUP_SIZE_CREATE_INFO_EXT;
	subgroupSizeCreateInfo.pNext                                                  = nullptr;
	subgroupSizeCreateInfo.requiredSubgroupSize                                   = 1;

	createInfo.pNext = &subgroupSizeCreateInfo;

	// check that the creation goes well.
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		SLIME_ERROR("Error when building the compute shader");
		return VK_NULL_HANDLE;
	}

	return shaderModule;
}

void vkutil::InitBackgroundPipelines()
{
	VulkanEngine& engine = VulkanEngine::Get();

	VkPipelineLayoutCreateInfo computeLayout{};
	computeLayout.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	computeLayout.pNext          = nullptr;
	computeLayout.pSetLayouts    = &engine.m_drawImageDescriptorLayout;
	computeLayout.setLayoutCount = 1;

	VkPushConstantRange pushConstant{};
	pushConstant.offset     = 0;
	pushConstant.size       = sizeof(ComputePushConstants);
	pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	computeLayout.pPushConstantRanges    = &pushConstant;
	computeLayout.pushConstantRangeCount = 1;

	VK_CHECK(vkCreatePipelineLayout(engine.m_device, &computeLayout, nullptr, &engine.m_gradientPipelineLayout));

	VkShaderModule gradientShader   = vkutil::LoadShaderModule(ResourceManager::GetVulkanShaderPath("Basic", ShaderStage::Compute).c_str(), engine.m_device);
	VkShaderModule skyShader        = vkutil::LoadShaderModule(ResourceManager::GetVulkanShaderPath("Sky", ShaderStage::Compute).c_str(), engine.m_device);
	VkShaderModule mandelbrotShader = vkutil::LoadShaderModule(ResourceManager::GetVulkanShaderPath("Mandelbrot", ShaderStage::Compute).c_str(), engine.m_device);
	VkShaderModule waterShader      = vkutil::LoadShaderModule(ResourceManager::GetVulkanShaderPath("water", ShaderStage::Compute).c_str(), engine.m_device);
	VkShaderModule raytracingShader = vkutil::LoadShaderModule(ResourceManager::GetVulkanShaderPath("Raytracing", ShaderStage::Compute).c_str(), engine.m_device);

	VkPipelineShaderStageCreateInfo stageinfo{};
	stageinfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageinfo.pNext  = nullptr;
	stageinfo.stage  = VK_SHADER_STAGE_COMPUTE_BIT;
	stageinfo.module = gradientShader;
	stageinfo.pName  = "main";

	VkComputePipelineCreateInfo computePipelineCreateInfo{};
	computePipelineCreateInfo.sType  = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	computePipelineCreateInfo.pNext  = nullptr;
	computePipelineCreateInfo.layout = engine.m_gradientPipelineLayout;
	computePipelineCreateInfo.stage  = stageinfo;

	ComputeEffect* gradient = engine.CreateComputeEffect("gradient", gradientShader, engine.m_gradientPipelineLayout, computePipelineCreateInfo);
	gradient->data.data1    = glm::vec4(1, 0, 0, 1);
	gradient->data.data2    = glm::vec4(0, 0, 1, 1);

	ComputeEffect* sky = engine.CreateComputeEffect("sky", skyShader, engine.m_gradientPipelineLayout, computePipelineCreateInfo);
	sky->data.data1    = glm::vec4(0.1, 0.2, 0.4, 0.97);

	engine.CreateComputeEffect("mandelbrot", mandelbrotShader, engine.m_gradientPipelineLayout, computePipelineCreateInfo);
	engine.CreateComputeEffect("water", waterShader, engine.m_gradientPipelineLayout, computePipelineCreateInfo);
	engine.CreateComputeEffect("raytracing", raytracingShader, engine.m_gradientPipelineLayout, computePipelineCreateInfo);

	engine.AddToDeletionQueue(
	    [&]()
	    {
		    for (auto effect : engine.backgroundEffects)
		    {
			    vkDestroyPipeline(engine.m_device, effect->pipeline, nullptr);
		    }
	    });
}

void vkutil::InitMeshPipeline()
{
	VulkanEngine& engine = VulkanEngine::Get();

	VkShaderModule meshVertexShader = vkutil::LoadShaderModule(ResourceManager::GetVulkanShaderPath("Mesh", ShaderStage::Vertex).c_str(), engine.m_device);
	VkShaderModule meshFragShader   = vkutil::LoadShaderModule(ResourceManager::GetVulkanShaderPath("Mesh", ShaderStage::Fragment).c_str(), engine.m_device);

	VkPushConstantRange matrixRange{};
	matrixRange.offset     = 0;
	matrixRange.size       = sizeof(GPUDrawPushConstants);
	matrixRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayout layouts[] = { engine.m_gpuSceneDataDescriptorLayout, engine.m_gltfMatDescriptorLayout };

	VkPipelineLayoutCreateInfo mesh_layout_info = vkinit::PipelineLayoutCreateInfo();
	mesh_layout_info.setLayoutCount             = 2;
	mesh_layout_info.pSetLayouts                = layouts;
	mesh_layout_info.pPushConstantRanges        = &matrixRange;
	mesh_layout_info.pushConstantRangeCount     = 1;

	VkPipelineLayout defaultLayout;
	VK_CHECK(vkCreatePipelineLayout(engine.m_device, &mesh_layout_info, nullptr, &defaultLayout));

	engine.m_gltfDefaultOpaque.layout      = defaultLayout;
	engine.m_gltfDefaultTranslucent.layout = defaultLayout;

	// build the stage-create-info for both vertex and fragment stages. This lets
	// the pipeline know the shader modules per stage
	vkutil::PipelineBuilder pipelineBuilder;
	pipelineBuilder.SetShaders(meshVertexShader, meshFragShader);
	pipelineBuilder.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	pipelineBuilder.SetPolygonMode(VK_POLYGON_MODE_FILL);
	pipelineBuilder.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
	pipelineBuilder.SetMultisamplingNone();
	pipelineBuilder.DisableBlending();
	pipelineBuilder.EnableDepthtest(true, VK_COMPARE_OP_LESS_OR_EQUAL);

	// render format
	pipelineBuilder.SetColorAttachmentFormat(engine.m_drawImage.imageFormat);
	pipelineBuilder.SetDepthFormat(engine.m_depthImage.imageFormat);

	// use the triangle layout we created
	pipelineBuilder.m_pipelineLayout = engine.m_gltfDefaultOpaque.layout;

	// finally build the pipeline
	engine.m_gltfDefaultOpaque.pipeline = pipelineBuilder.BuildPipeline(engine.m_device);
	engine.m_gltfDefaultOpaque.passType = MaterialPass::MainColor;

	// create the transparent variant
	pipelineBuilder.EnableBlendingAdditive();

	pipelineBuilder.EnableDepthtest(false, VK_COMPARE_OP_LESS_OR_EQUAL);

	engine.m_gltfDefaultTranslucent.pipeline = pipelineBuilder.BuildPipeline(engine.m_device);
	engine.m_gltfDefaultTranslucent.passType = MaterialPass::Transparent;

	vkDestroyShaderModule(engine.m_device, meshFragShader, nullptr);
	vkDestroyShaderModule(engine.m_device, meshVertexShader, nullptr);

	engine.AddToDeletionQueue(
	    [&]()
	    {
		    vkDestroyPipelineLayout(engine.m_device, engine.m_gradientPipelineLayout, nullptr);
		    vkDestroyPipelineLayout(engine.m_device, engine.m_gltfDefaultOpaque.layout, nullptr);

		    vkDestroyPipeline(engine.m_device, engine.m_gltfDefaultTranslucent.pipeline, nullptr);
		    vkDestroyPipeline(engine.m_device, engine.m_gltfDefaultOpaque.pipeline, nullptr);
	    });
}

void vkutil::InitBindlessPipeline()
{
	VulkanEngine& engine                  = VulkanEngine::Get();
	VkDescriptorSetLayout& bindlessLayout = engine.m_bindlessWriter.bindlessLayout;

	VkShaderModule bindlessVertexShader = vkutil::LoadShaderModule(ResourceManager::GetVulkanShaderPath("BasicBindless", ShaderStage::Vertex).c_str(), engine.m_device);
	VkShaderModule bindlessFragShader   = vkutil::LoadShaderModule(ResourceManager::GetVulkanShaderPath("BasicBindless", ShaderStage::Fragment).c_str(), engine.m_device);

	// build the pipeline layout that controls the inputs/outputs of the shader
	// we are going to be using a bindingless texture array, so we dont need to
	// specify any descriptors in the layout itself

	VkPipelineLayoutCreateInfo layoutInfo = vkinit::PipelineLayoutCreateInfo();
	layoutInfo.setLayoutCount             = 1;
	layoutInfo.pSetLayouts                = &bindlessLayout;

	VK_CHECK(vkCreatePipelineLayout(engine.m_device, &layoutInfo, nullptr, &engine.m_bindlessPipelineLayout));

	// build the stage-create-info for both vertex and fragment stages. This lets
	// the pipeline know the shader modules per stage and their entry points
	vkutil::PipelineBuilder pipelineBuilder;
	pipelineBuilder.SetShaders(bindlessVertexShader, bindlessFragShader);
	pipelineBuilder.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	pipelineBuilder.SetPolygonMode(VK_POLYGON_MODE_FILL);
	pipelineBuilder.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
	pipelineBuilder.SetMultisamplingNone();
	pipelineBuilder.DisableBlending();
	pipelineBuilder.EnableDepthtest(true, VK_COMPARE_OP_LESS_OR_EQUAL);

	// render format
	pipelineBuilder.SetColorAttachmentFormat(engine.m_drawImage.imageFormat);
	pipelineBuilder.SetDepthFormat(engine.m_depthImage.imageFormat);

	pipelineBuilder.m_pipelineLayout = engine.m_bindlessPipelineLayout;

	engine.m_bindlessPipeline = pipelineBuilder.BuildPipeline(engine.m_device);

	vkDestroyShaderModule(engine.m_device, bindlessFragShader, nullptr);
	vkDestroyShaderModule(engine.m_device, bindlessVertexShader, nullptr);

	engine.AddToDeletionQueue(
	    [&]()
	    {
		    vkDestroyPipelineLayout(engine.m_device, engine.m_bindlessPipelineLayout, nullptr);
		    vkDestroyPipeline(engine.m_device, engine.m_bindlessPipeline, nullptr);
	    });
}

void vkutil::PipelineBuilder::Clear()
{
	// clear all of th.m_gltfBindlessOpaque.layout;ith their correct stype

	m_inputAssembly = { .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };

	m_rasterizer = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };

	m_colorBlendAttachment = {};

	m_multisampling = { .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };

	m_pipelineLayout = {};

	m_depthStencil = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };

	m_renderInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };

	m_shaderStages.clear();
}

VkPipeline vkutil::PipelineBuilder::BuildPipeline(VkDevice device)
{
	// make viewport state from our stored viewport and scissor.
	// at the moment we wont support multiple viewports or scissors
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType                             = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.pNext                             = nullptr;

	viewportState.viewportCount = 1;
	viewportState.scissorCount  = 1;

	// setup dummy color blending. We arent using transparent objects yet
	// the blending is just "no blend", but we do write to the color attachment
	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType                               = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.pNext                               = nullptr;

	colorBlending.logicOpEnable   = VK_FALSE;
	colorBlending.logicOp         = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments    = &m_colorBlendAttachment;

	// completely clear VertexInputStateCreateInfo, as we have no need for it
	VkPipelineVertexInputStateCreateInfo _vertexInputInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };

	// build the actual pipeline
	// we now use all of the info structs we have been writing into into this one
	// to create the pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo = { .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
	// connect the renderInfo to the pNext extension mechanism
	pipelineInfo.pNext = &m_renderInfo;

	pipelineInfo.stageCount          = (uint32_t)m_shaderStages.size();
	pipelineInfo.pStages             = m_shaderStages.data();
	pipelineInfo.pVertexInputState   = &_vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &m_inputAssembly;
	pipelineInfo.pViewportState      = &viewportState;
	pipelineInfo.pRasterizationState = &m_rasterizer;
	pipelineInfo.pMultisampleState   = &m_multisampling;
	pipelineInfo.pColorBlendState    = &colorBlending;
	pipelineInfo.pDepthStencilState  = &m_depthStencil;
	pipelineInfo.layout              = m_pipelineLayout;

	VkDynamicState state[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineDynamicStateCreateInfo dynamicInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
	dynamicInfo.pDynamicStates                   = &state[0];
	dynamicInfo.dynamicStateCount                = 2;

	pipelineInfo.pDynamicState = &dynamicInfo;

	// its easy to error out on create graphics pipeline, so we handle it a bit
	// better than the common VK_CHECK case
	VkPipeline newPipeline;
	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newPipeline) != VK_SUCCESS)
	{
		SLIME_ERROR("Failed to create the pipeline");
		return VK_NULL_HANDLE; // failed to create graphics pipeline
	}

	return newPipeline;
}

void vkutil::PipelineBuilder::SetShaders(VkShaderModule vertexShader, VkShaderModule fragmentShader)
{
	m_shaderStages.clear();
	m_shaderStages.push_back(vkinit::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, vertexShader));
	m_shaderStages.push_back(vkinit::PipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, fragmentShader));
}

void vkutil::PipelineBuilder::SetInputTopology(VkPrimitiveTopology topology)
{
	m_inputAssembly.topology               = topology;
	m_inputAssembly.primitiveRestartEnable = VK_FALSE;
}

void vkutil::PipelineBuilder::SetPolygonMode(VkPolygonMode mode)
{
	m_rasterizer.polygonMode = mode;
	m_rasterizer.lineWidth   = 1.f;
}

void vkutil::PipelineBuilder::SetCullMode(VkCullModeFlags cullMode, VkFrontFace frontFace)
{
	m_rasterizer.cullMode  = cullMode;
	m_rasterizer.frontFace = frontFace;
}

void vkutil::PipelineBuilder::SetMultisamplingNone()
{
	m_multisampling.sampleShadingEnable = VK_FALSE;
	// multisampling defaulted to no multisampling (1 sample per pixel)
	m_multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	m_multisampling.minSampleShading     = 1.0f;
	m_multisampling.pSampleMask          = nullptr;
	// no alpha to coverage either
	m_multisampling.alphaToCoverageEnable = VK_FALSE;
	m_multisampling.alphaToOneEnable      = VK_FALSE;
}

void vkutil::PipelineBuilder::DisableBlending()
{
	// default write mask
	m_colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	// no blending
	m_colorBlendAttachment.blendEnable = VK_FALSE;
}

void vkutil::PipelineBuilder::SetColorAttachmentFormat(VkFormat format)
{
	m_colorAttachmentformat = format;
	// connect the format to the renderInfo  structure
	m_renderInfo.colorAttachmentCount    = 1;
	m_renderInfo.pColorAttachmentFormats = &m_colorAttachmentformat;
}

void vkutil::PipelineBuilder::SetDepthFormat(VkFormat format)
{
	m_renderInfo.depthAttachmentFormat = format;
}

void vkutil::PipelineBuilder::DisableDepthtest()
{
	m_depthStencil.depthTestEnable       = VK_FALSE;
	m_depthStencil.depthWriteEnable      = VK_FALSE;
	m_depthStencil.depthCompareOp        = VK_COMPARE_OP_NEVER;
	m_depthStencil.depthBoundsTestEnable = VK_FALSE;
	m_depthStencil.stencilTestEnable     = VK_FALSE;
	m_depthStencil.front                 = {};
	m_depthStencil.back                  = {};
	m_depthStencil.minDepthBounds        = 0.f;
	m_depthStencil.maxDepthBounds        = 1.f;
}

void vkutil::PipelineBuilder::EnableDepthtest(bool depthWriteEnable, VkCompareOp op)
{
	m_depthStencil.depthTestEnable       = VK_TRUE;
	m_depthStencil.depthWriteEnable      = depthWriteEnable;
	m_depthStencil.depthCompareOp        = op;
	m_depthStencil.depthBoundsTestEnable = VK_FALSE;
	m_depthStencil.stencilTestEnable     = VK_FALSE;
	m_depthStencil.front                 = {};
	m_depthStencil.back                  = {};
	m_depthStencil.minDepthBounds        = 0.f;
	m_depthStencil.maxDepthBounds        = 1.f;
}

void vkutil::PipelineBuilder::EnableBlendingAdditive()
{
	m_colorBlendAttachment.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	m_colorBlendAttachment.blendEnable         = VK_TRUE;
	m_colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	m_colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
	m_colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;
	m_colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	m_colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	m_colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;
}
