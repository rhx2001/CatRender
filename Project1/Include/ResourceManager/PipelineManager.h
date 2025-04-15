#pragma once
#include <array>
#include <deque>
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_core.h>

struct PipelineConfig {
	// Shader 模块
	VkShaderModule vertexShader = VK_NULL_HANDLE;
	VkShaderModule fragmentShader = VK_NULL_HANDLE;
	VkShaderModule geometryShader = VK_NULL_HANDLE;
	VkShaderModule tessControlShader = VK_NULL_HANDLE;
	VkShaderModule tessEvalShader = VK_NULL_HANDLE;

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
	bool operator==(const PipelineConfig& other) const;

	// 创建散列值函数
	size_t hashValue() const;
};

struct PipelineConfigHasher {
	size_t operator()(const struct PipelineConfig& config) const;
};
template <class T>
void hashCombine(std::size_t& seed, const T& v) {
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}
class PipelineManager
{
	
public:

	PipelineManager(VkDevice& device) :m_device(device) {}
	~PipelineManager() { cleanup(); }

public:
	VkPipeline getPipeline(const PipelineConfig& config);
	class Builder {
	public:
		Builder(PipelineManager* manager);

		// 设置着色器
		Builder& setShader(VkShaderStageFlagBits stage, const ShaderModule* shader);
		Builder& setVertexShader(const ShaderModule* shader);
		Builder& setFragmentShader(const ShaderModule* shader);

		// 设置顶点输入
		Builder& setVertexInput(const std::vector<VkVertexInputBindingDescription>& bindings,
			const std::vector<VkVertexInputAttributeDescription>& attributes);

		// 设置拓扑
		Builder& setTopology(VkPrimitiveTopology topology);

		// 设置光栅化状态
		Builder& setCullMode(VkCullModeFlags cullMode);
		Builder& setPolygonMode(VkPolygonMode polygonMode);

		// 设置深度测试
		Builder& setDepthTest(bool enable);
		Builder& setDepthWrite(bool enable);
		Builder& setDepthCompareOp(VkCompareOp compareOp);

		// 设置混合状态
		Builder& setBlending(bool enable);
		Builder& setBlendFactors(VkBlendFactor srcColor, VkBlendFactor dstColor,
			VkBlendFactor srcAlpha, VkBlendFactor dstAlpha);

		// 设置布局和渲染通道
		Builder& setPipelineLayout(VkPipelineLayout layout);
		Builder& setRenderPass(VkRenderPass renderPass, uint32_t subpass = 0);

		// 构建管线
		VkPipeline build();

	private:
		PipelineManager* manager;
		PipelineConfig config;
	};

	Builder createBuilder();

	// 清理所有缓存的管线
	void cleanup();
private:
	VkDevice& m_device;
	std::unordered_map<size_t, VkPipeline> pipelineCache;

	// 内部创建管线的函数
	VkPipeline createPipeline(const PipelineConfig& config);
};
