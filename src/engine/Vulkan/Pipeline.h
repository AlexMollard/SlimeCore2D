#pragma once

#include "engine/Vulkan/volk.h"
#include <vector>

namespace vkutil
{
VkShaderModule LoadShaderModule(const char* filePath, VkDevice device);

class PipelineBuilder
{
public:
	std::vector<VkPipelineShaderStageCreateInfo> m_shaderStages;

	VkPipelineInputAssemblyStateCreateInfo m_inputAssembly;
	VkPipelineRasterizationStateCreateInfo m_rasterizer;
	VkPipelineColorBlendAttachmentState m_colorBlendAttachment;
	VkPipelineMultisampleStateCreateInfo m_multisampling;
	VkPipelineLayout m_pipelineLayout;
	VkPipelineDepthStencilStateCreateInfo m_depthStencil;
	VkPipelineRenderingCreateInfo m_renderInfo;
	VkFormat m_colorAttachmentformat;

	PipelineBuilder()
	{
		Clear();
	}

	void Clear();

	VkPipeline BuildPipeline(VkDevice device);
	void SetShaders(VkShaderModule vertexShader, VkShaderModule fragmentShader);
	void SetInputTopology(VkPrimitiveTopology topology);
	void SetPolygonMode(VkPolygonMode mode);
	void SetCullMode(VkCullModeFlags cullMode, VkFrontFace frontFace);
	void SetMultisamplingNone();
	void DisableBlending();
	void SetColorAttachmentFormat(VkFormat format);
	void SetDepthFormat(VkFormat format);
	void DisableDepthtest();
	void EnableDepthtest(bool depthWriteEnable, VkCompareOp op);
	void EnableBlendingAdditive();
};

void InitBackgroundPipelines();
void InitMeshPipeline();
void InitBindlessPipeline();
} // namespace vkutil