#pragma once
#include <array>
#include <deque>
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_core.h>

class PipelineManager
{
public:
	struct PipelineInfo
	{
		VkShaderModule vertexShaderModule;
		VkShaderModule fragmentShaderModule;
		VkVertexInputBindingDescription bindingDescription;
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions;
		VkExtent2D swapChainExtent;
		VkPipelineRasterizationStateCreateInfo rasterizer;
		VkRenderPass renderPass;
		std::vector<VkDescriptorSetLayout> layouts;
		std::string PresetName;
		std::string RenderPassName;
		std::string ShaderClassName;
		bool bRasterizer = true;
		bool bMultiSampling = false;
	};
public:
	PipelineManager(VkDevice& device):m_device(device){}
	~PipelineManager() = default;
	VkPipeline GetPipeline(PipelineInfo createInfo);
	VkPipeline CreatePipeline(PipelineInfo createInfo);

public:
	VkDevice& m_device;

private:
	uint32_t PipelineID = -1;
	uint32_t GenerateID() { return PipelineID++; }

	std::unordered_map<std::string, VkPipeline> PipelineCache;
	std::deque<VkPipelineLayout> PipelineLayoutCache;
};
