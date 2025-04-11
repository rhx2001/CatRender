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
	//默认配置
	static VkPipelineDepthStencilStateCreateInfo DefaultDepthStencilState() {
		VkPipelineDepthStencilStateCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		info.depthTestEnable = VK_TRUE;
		info.depthWriteEnable = VK_TRUE;
		info.depthCompareOp = VK_COMPARE_OP_LESS;
		info.depthBoundsTestEnable = VK_FALSE;
		info.stencilTestEnable = VK_FALSE;
		return info;
	}

	static VkPipelineColorBlendStateCreateInfo DefaultColorBlendState() {
		VkPipelineColorBlendAttachmentState attachment{};
		attachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT |
			VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
		attachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		info.attachmentCount = 1;
		info.pAttachments = &attachment;
		return info;
	}


	struct PipelineInfo {
		// Shader 模块
		VkShaderModule vertexShader = VK_NULL_HANDLE;
		VkShaderModule fragmentShader = VK_NULL_HANDLE;
		VkShaderModule meshShader = VK_NULL_HANDLE; // 可选

		// 顶点输入
		VkVertexInputBindingDescription bindingDescription;
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions;
		uint32_t attributeCount = 0;

		// 渲染流程
		VkRenderPass renderPass = VK_NULL_HANDLE;
		uint32_t subpass = 0;

		// 动态状态
		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		// 固定功能状态
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		VkPipelineDepthStencilStateCreateInfo depthStencil = DefaultDepthStencilState();
		VkPipelineColorBlendStateCreateInfo colorBlend = DefaultColorBlendState();
		VkPipelineMultisampleStateCreateInfo multisampling{};

		// 描述符布局
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

		// 标识符（用于调试）
		std::string debugName;
	};

public:
	PipelineManager(VkDevice& device):m_device(device){}
	~PipelineManager() = default;
	VkPipeline GetPipeline(PipelineInfo createInfo);
	VkPipeline GetOrCreatePipeline(const PipelineInfo& info);

public:
	VkDevice& m_device;

private:
	struct PipelineKey {
		size_t vertexShaderHash;
		size_t fragmentShaderHash;
		size_t meshShaderHash;
		size_t vertexInputHash;
		size_t renderPassHash;
		size_t descriptorLayoutsHash;
		size_t  rasterizerHash;
		size_t  depthStencilHash;
		size_t  colorBlendHash;

		bool operator==(const PipelineKey& other) const {
			return
				vertexShaderHash == other.vertexShaderHash &&
				fragmentShaderHash == other.fragmentShaderHash &&
				meshShaderHash == other.meshShaderHash &&
				vertexInputHash == other.vertexInputHash &&
				renderPassHash == other.renderPassHash &&
				descriptorLayoutsHash == other.descriptorLayoutsHash &&
				rasterizerHash == other.rasterizerHash &&
				depthStencilHash == other.depthStencilHash &&
				colorBlendHash == other.colorBlendHash;
		}
	};


	struct PipelineKeyHasher {
		size_t operator()(const PipelineKey& key) const {
			size_t seed = 0;
			auto hash_combine = [&seed](const auto& val) {
				using SeedType = std::decay_t<decltype(seed)>;
				seed = static_cast<SeedType>(
					seed ^ (std::hash<std::decay_t<decltype(val)>>{}(val)+0x9e3779b9 + (seed << 6) + (seed >> 2))
					);
				};
			hash_combine(key.vertexShaderHash);
			hash_combine(key.fragmentShaderHash);
			hash_combine(key.meshShaderHash);
			hash_combine(key.vertexInputHash);
			hash_combine(key.renderPassHash);
			hash_combine(key.descriptorLayoutsHash);
			hash_combine(key.rasterizerHash);
			hash_combine(key.depthStencilHash);
			hash_combine(key.colorBlendHash);
			return seed;
		}
	};

	PipelineKey GenerateKey(const PipelineInfo& info);
	VkPipeline CreatePipeline(const PipelineInfo& info);
	VkPipelineLayout GetOrCreatePipelineLayout(const std::vector<VkDescriptorSetLayout>& layouts);

	std::unordered_map<PipelineKey, VkPipeline, PipelineKeyHasher> m_pipelineCache;
	std::unordered_map<size_t, VkPipelineLayout> m_pipelineLayouts;



};
