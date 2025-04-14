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
	struct PipelineInfo {
		// Shader 模块
		VkShaderModule vertexShader = VK_NULL_HANDLE;
		VkShaderModule fragmentShader = VK_NULL_HANDLE;
		VkShaderModule meshShader = VK_NULL_HANDLE; // 可选

		// 顶点输入状态
		struct {
			std::vector<VkVertexInputBindingDescription> bindings;
			std::vector<VkVertexInputAttributeDescription> attributes;
		} vertexInput;

		// 输入装配状态
		VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		bool primitiveRestartEnable = false;

		// 视口和裁剪状态
		bool dynamicViewportState = true;
		std::vector<VkViewport> viewports;
		std::vector<VkRect2D> scissors;

		//光栅化状态
		struct {
			VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
			VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT;
			VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
			float lineWidth = 1.0f;
			bool depthClampEnable = false;
			bool rasterizerDiscardEnable = false;
			bool depthBiasEnable = false;
		} rasterization;


		// 多重采样状态
		struct {
			VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
			bool sampleShadingEnable = false;
			float minSampleShading = 1.0f;
		} multisample;

		// 深度模板状态
		struct {
			bool depthTestEnable = true;
			bool depthWriteEnable = true;
			VkCompareOp depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
			bool depthBoundsTestEnable = false;
			bool stencilTestEnable = false;
		} depthStencil;

		// 混合状态
		struct {
			bool blendEnable = false;
			VkBlendFactor srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
			VkBlendFactor dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
			VkBlendOp colorBlendOp = VK_BLEND_OP_ADD;
			VkBlendFactor srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			VkBlendFactor dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
			VkBlendOp alphaBlendOp = VK_BLEND_OP_ADD;
			VkColorComponentFlags colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
				VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		} colorBlend;

		// 动态状态
		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		// 管道布局
		VkPipelineLayout layout = VK_NULL_HANDLE;

		// 渲染通道
		VkRenderPass renderPass = VK_NULL_HANDLE;
		uint32_t subpass = 0;

		// 比较操作符
		bool operator==(const PipelineInfo& other) const;

		// 创建散列值函数
		size_t hashValue() const;
	};

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

	PipelineManager(VkDevice& device) :m_device(device) {}
	~PipelineManager() = default;
	VkPipeline GetPipeline(PipelineInfo createInfo);
	VkPipeline GetOrCreatePipeline(const PipelineInfo& info);

	PipelineKey GenerateKey(const PipelineInfo& info);
	VkPipeline CreatePipeline(const PipelineInfo& info);
	VkPipelineLayout GetOrCreatePipelineLayout(const std::vector<VkDescriptorSetLayout>& layouts);

	std::unordered_map<PipelineKey, VkPipeline, PipelineKeyHasher> m_pipelineCache;
	std::unordered_map<size_t, VkPipelineLayout> m_pipelineLayouts;



};
