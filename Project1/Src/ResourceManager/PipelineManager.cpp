#include <stdexcept>
#include <ResourceManager/PipelineManager.h>

VkPipeline PipelineManager::GetPipeline(PipelineInfo createInfo)
{
	//TODO:完善对比与创建的过程：
	if (PipelineCache.find(createInfo))
	{
		return PipelineCache[createInfo.PresetName];
	}
	else
	{
		return CreatePipeline(createInfo);
	}
}

VkPipeline PipelineManager::CreatePipeline(const PipelineInfo& info) {
    // 1. 准备 Shader 阶段
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

    if (info.vertexShader) {
        VkPipelineShaderStageCreateInfo stageInfo{};
        stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        stageInfo.module = info.vertexShader;
        stageInfo.pName = "main";
        shaderStages.push_back(stageInfo);
    }

    if (info.fragmentShader) {
        VkPipelineShaderStageCreateInfo stageInfo{};
        stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        stageInfo.module = info.fragmentShader;
        stageInfo.pName = "main";
        shaderStages.push_back(stageInfo);
    }

    if (info.meshShader) {
        VkPipelineShaderStageCreateInfo stageInfo{};
        stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageInfo.stage = VK_SHADER_STAGE_MESH_BIT_EXT;
        stageInfo.module = info.meshShader;
        stageInfo.pName = "main";
        shaderStages.push_back(stageInfo);
    }

    if (shaderStages.empty()) {
        throw std::runtime_error("No shader stages provided!");
    }

    // 2. 顶点输入状态
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &info.bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = info.attributeCount;
    vertexInputInfo.pVertexAttributeDescriptions = info.attributeDescriptions.data();

    // 3. 动态状态
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(info.dynamicStates.size());
    dynamicState.pDynamicStates = info.dynamicStates.data();

    // 4. 获取或创建 PipelineLayout
    VkPipelineLayout pipelineLayout = GetOrCreatePipelineLayout(info.descriptorSetLayouts);

    // 5. 创建 Pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &info.inputAssembly;
    pipelineInfo.pRasterizationState = &info.rasterizer;
    pipelineInfo.pDepthStencilState = &info.depthStencil;
    pipelineInfo.pColorBlendState = &info.colorBlend;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.renderPass = info.renderPass;
    pipelineInfo.subpass = info.subpass;

    VkPipeline pipeline;
    if (vkCreateGraphicsPipelines(
        m_device,
        VK_NULL_HANDLE,
        1,
        &pipelineInfo,
        nullptr,
        &pipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create graphics pipeline!");
    }

    // 6. 可选：设置调试名称
    if (!info.debugName.empty()) {
        VkDebugUtilsObjectNameInfoEXT nameInfo{};
        nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        nameInfo.objectType = VK_OBJECT_TYPE_PIPELINE;
        nameInfo.objectHandle = (uint64_t)pipeline;
        nameInfo.pObjectName = info.debugName.c_str();
        vkSetDebugUtilsObjectNameEXT(m_device, &nameInfo);
    }

    return pipeline;
}

VkPipeline PipelineManager::GetOrCreatePipeline(const PipelineInfo& info) {
    PipelineKey key = GenerateKey(info);

    if (auto it = m_pipelineCache.find(key); it != m_pipelineCache.end()) {
        return it->second;
    }

    VkPipeline pipeline = CreatePipeline(info);
    m_pipelineCache[key] = pipeline;
    return pipeline;
}


PipelineManager::PipelineKey PipelineManager::GenerateKey(const PipelineInfo& info) {
	PipelineKey key{};

	// 哈希 Shader 模块
	key.vertexShaderHash = std::hash<VkShaderModule>{}(info.vertexShader);
	key.fragmentShaderHash = std::hash<VkShaderModule>{}(info.fragmentShader);
	key.meshShaderHash = std::hash<VkShaderModule>{}(info.meshShader);

	// 哈希顶点输入
	size_t vertexInputHash = 0;
	auto hash_combine = [](size_t& seed, const auto& val) {
		seed ^= std::hash<std::decay_t<decltype(val)>>{}(val)+0x9e3779b9 + (seed << 6) + (seed >> 2);
		};
	hash_combine(vertexInputHash, info.bindingDescription.binding);
	hash_combine(vertexInputHash, info.bindingDescription.stride);
	hash_combine(vertexInputHash, info.bindingDescription.inputRate);
	for (uint32_t i = 0; i < info.attributeCount; ++i) {
		const auto& attr = info.attributeDescriptions[i];
		hash_combine(vertexInputHash, attr.location);
		hash_combine(vertexInputHash, attr.binding);
		hash_combine(vertexInputHash, attr.format);
		hash_combine(vertexInputHash, attr.offset);
	}
	key.vertexInputHash = vertexInputHash;

	// 哈希 RenderPass 和描述符布局
	key.renderPassHash = std::hash<VkRenderPass>{}(info.renderPass);
	key.descriptorLayoutsHash = 0;
	for (auto layout : info.descriptorSetLayouts) {
		hash_combine(key.descriptorLayoutsHash, std::hash<VkDescriptorSetLayout>{}(layout));
	}

	// 哈希固定功能状态
	key.rasterizerHash = 0;
	hash_combine(key.rasterizerHash, info.rasterizer.polygonMode);
	hash_combine(key.rasterizerHash, info.rasterizer.cullMode);
	hash_combine(key.rasterizerHash, info.rasterizer.frontFace);
	hash_combine(key.rasterizerHash, info.rasterizer.depthClampEnable);

	key.depthStencilHash = 0;
	hash_combine(key.depthStencilHash, info.depthStencil.depthTestEnable);
	hash_combine(key.depthStencilHash, info.depthStencil.depthWriteEnable);
	hash_combine(key.depthStencilHash, info.depthStencil.depthCompareOp);

	key.colorBlendHash = 0;
	for (uint32_t i = 0; i < info.colorBlend.attachmentCount; ++i) {
		const auto& attach = info.colorBlend.pAttachments[i];
		hash_combine(key.colorBlendHash, attach.blendEnable);
		hash_combine(key.colorBlendHash, attach.srcColorBlendFactor);
		hash_combine(key.colorBlendHash, attach.dstColorBlendFactor);
	}

	return key;
}