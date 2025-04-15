#include <stdexcept>
#include <ResourceManager/PipelineManager.h>


// 计算管线配置的哈希值
bool PipelineConfig::operator==(const PipelineConfig& other) const {
    // 比较所有字段...
    if (vertexShader != other.vertexShader) return false;
    if (fragmentShader != other.fragmentShader) return false;
    if (geometryShader != other.geometryShader) return false;
    if (tessControlShader != other.tessControlShader) return false;
    if (tessEvalShader != other.tessEvalShader) return false;

    // 比较顶点输入状态
    if (vertexInput.bindings.size() != other.vertexInput.bindings.size()) return false;
    for (size_t i = 0; i < vertexInput.bindings.size(); i++) {
        if (memcmp(&vertexInput.bindings[i], &other.vertexInput.bindings[i],
            sizeof(VkVertexInputBindingDescription)) != 0) {
            return false;
        }
    }

    if (vertexInput.attributes.size() != other.vertexInput.attributes.size()) return false;
    for (size_t i = 0; i < vertexInput.attributes.size(); i++) {
        if (memcmp(&vertexInput.attributes[i], &other.vertexInput.attributes[i],
            sizeof(VkVertexInputAttributeDescription)) != 0) {
            return false;
        }
    }

    // 其他比较...
    if (topology != other.topology) return false;
    if (primitiveRestartEnable != other.primitiveRestartEnable) return false;

    // 光栅化状态比较
    if (rasterization.polygonMode != other.rasterization.polygonMode) return false;
    if (rasterization.cullMode != other.rasterization.cullMode) return false;
    if (rasterization.frontFace != other.rasterization.frontFace) return false;
    if (rasterization.lineWidth != other.rasterization.lineWidth) return false;
    if (rasterization.depthClampEnable != other.rasterization.depthClampEnable) return false;
    if (rasterization.rasterizerDiscardEnable != other.rasterization.rasterizerDiscardEnable) return false;
    if (rasterization.depthBiasEnable != other.rasterization.depthBiasEnable) return false;

    // 深度模板状态比较
    if (depthStencil.depthTestEnable != other.depthStencil.depthTestEnable) return false;
    if (depthStencil.depthWriteEnable != other.depthStencil.depthWriteEnable) return false;
    if (depthStencil.depthCompareOp != other.depthStencil.depthCompareOp) return false;
    if (depthStencil.depthBoundsTestEnable != other.depthStencil.depthBoundsTestEnable) return false;
    if (depthStencil.stencilTestEnable != other.depthStencil.stencilTestEnable) return false;

    // 混合状态比较
    if (colorBlend.blendEnable != other.colorBlend.blendEnable) return false;
    if (colorBlend.srcColorBlendFactor != other.colorBlend.srcColorBlendFactor) return false;
    if (colorBlend.dstColorBlendFactor != other.colorBlend.dstColorBlendFactor) return false;
    if (colorBlend.colorBlendOp != other.colorBlend.colorBlendOp) return false;
    if (colorBlend.srcAlphaBlendFactor != other.colorBlend.srcAlphaBlendFactor) return false;
    if (colorBlend.dstAlphaBlendFactor != other.colorBlend.dstAlphaBlendFactor) return false;
    if (colorBlend.alphaBlendOp != other.colorBlend.alphaBlendOp) return false;
    if (colorBlend.colorWriteMask != other.colorBlend.colorWriteMask) return false;

    // 渲染通道和子通道比较
    if (renderPass != other.renderPass) return false;
    if (subpass != other.subpass) return false;

    // 管道布局比较
    if (layout != other.layout) return false;

    return true;
}

size_t PipelineConfig::hashValue() const {
    size_t hash = 0;

    // 添加着色器模块到哈希
    hashCombine(hash, (size_t)vertexShader);
    hashCombine(hash, (size_t)fragmentShader);
    hashCombine(hash, (size_t)geometryShader);
    hashCombine(hash, (size_t)tessControlShader);
    hashCombine(hash, (size_t)tessEvalShader);

    // 添加顶点输入配置到哈希
    for (const auto& binding : vertexInput.bindings) {
        hashCombine(hash, binding.binding);
        hashCombine(hash, binding.stride);
        hashCombine(hash, binding.inputRate);
    }

    for (const auto& attr : vertexInput.attributes) {
        hashCombine(hash, attr.location);
        hashCombine(hash, attr.binding);
        hashCombine(hash, attr.format);
        hashCombine(hash, attr.offset);
    }

    // 添加拓扑
    hashCombine(hash, topology);
    hashCombine(hash, primitiveRestartEnable);

    // 添加光栅化状态
    hashCombine(hash, rasterization.polygonMode);
    hashCombine(hash, rasterization.cullMode);
    hashCombine(hash, rasterization.frontFace);
    hashCombine(hash, rasterization.lineWidth);
    hashCombine(hash, rasterization.depthClampEnable);
    hashCombine(hash, rasterization.rasterizerDiscardEnable);
    hashCombine(hash, rasterization.depthBiasEnable);

    // 添加深度模板状态
    hashCombine(hash, depthStencil.depthTestEnable);
    hashCombine(hash, depthStencil.depthWriteEnable);
    hashCombine(hash, depthStencil.depthCompareOp);
    hashCombine(hash, depthStencil.depthBoundsTestEnable);
    hashCombine(hash, depthStencil.stencilTestEnable);

    // 添加混合状态
    hashCombine(hash, colorBlend.blendEnable);
    hashCombine(hash, colorBlend.srcColorBlendFactor);
    hashCombine(hash, colorBlend.dstColorBlendFactor);
    hashCombine(hash, colorBlend.colorBlendOp);
    hashCombine(hash, colorBlend.srcAlphaBlendFactor);
    hashCombine(hash, colorBlend.dstAlphaBlendFactor);
    hashCombine(hash, colorBlend.alphaBlendOp);
    hashCombine(hash, colorBlend.colorWriteMask);

    // 添加渲染通道和子通道
    hashCombine(hash, (size_t)renderPass);
    hashCombine(hash, subpass);

    // 添加管道布局
    hashCombine(hash, (size_t)layout);

    return hash;
}

size_t PipelineConfigHasher::operator()(const PipelineConfig& config) const {
    return config.hashValue();
}

VkPipeline PipelineManager::createPipeline(const PipelineConfig& config) {
    // 创建着色器阶段信息
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

    // 顶点着色器
    if (config.vertexShader != VK_NULL_HANDLE) {
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = config.vertexShader;
        vertShaderStageInfo.pName = "main";
        shaderStages.push_back(vertShaderStageInfo);
    }

    // 片段着色器
    if (config.fragmentShader != VK_NULL_HANDLE) {
        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = config.fragmentShader;
        fragShaderStageInfo.pName = "main";
        shaderStages.push_back(fragShaderStageInfo);
    }

    // 几何着色器
    if (config.geometryShader != VK_NULL_HANDLE) {
        VkPipelineShaderStageCreateInfo geomShaderStageInfo{};
        geomShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        geomShaderStageInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
        geomShaderStageInfo.module = config.geometryShader;
        geomShaderStageInfo.pName = "main";
        shaderStages.push_back(geomShaderStageInfo);
    }

    // 细分控制着色器
    if (config.tessControlShader != VK_NULL_HANDLE) {
        VkPipelineShaderStageCreateInfo tessControlStageInfo{};
        tessControlStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        tessControlStageInfo.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        tessControlStageInfo.module = config.tessControlShader;
        tessControlStageInfo.pName = "main";
        shaderStages.push_back(tessControlStageInfo);
    }

    // 细分评估着色器
    if (config.tessEvalShader != VK_NULL_HANDLE) {
        VkPipelineShaderStageCreateInfo tessEvalStageInfo{};
        tessEvalStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        tessEvalStageInfo.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        tessEvalStageInfo.module = config.tessEvalShader;
        tessEvalStageInfo.pName = "main";
        shaderStages.push_back(tessEvalStageInfo);
    }
	// 顶点输入状态
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(config.vertexInput.bindings.size());
	vertexInputInfo.pVertexBindingDescriptions = config.vertexInput.bindings.data();
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(config.vertexInput.attributes.size());
	vertexInputInfo.pVertexAttributeDescriptions = config.vertexInput.attributes.data();
	// 输入装配状态
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = config.topology;
	inputAssembly.primitiveRestartEnable = config.primitiveRestartEnable;

    // 视口状态
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;

    if (config.dynamicViewportState) {
        // 如果使用动态视口，只需设置计数
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;
    }
    else {
        // 否则使用固定视口
        viewportState.viewportCount = static_cast<uint32_t>(config.viewports.size());
        viewportState.pViewports = config.viewports.data();
        viewportState.scissorCount = static_cast<uint32_t>(config.scissors.size());
        viewportState.pScissors = config.scissors.data();
    }

    // 光栅化状态
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = config.rasterization.depthClampEnable;
    rasterizer.rasterizerDiscardEnable = config.rasterization.rasterizerDiscardEnable;
    rasterizer.polygonMode = config.rasterization.polygonMode;
    rasterizer.lineWidth = config.rasterization.lineWidth;
    rasterizer.cullMode = config.rasterization.cullMode;
    rasterizer.frontFace = config.rasterization.frontFace;
    rasterizer.depthBiasEnable = config.rasterization.depthBiasEnable;

    // 多重采样状态
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = config.multisample.sampleShadingEnable;
    multisampling.rasterizationSamples = config.multisample.samples;
    multisampling.minSampleShading = config.multisample.minSampleShading;

    // 深度模板状态
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = config.depthStencil.depthTestEnable;
    depthStencil.depthWriteEnable = config.depthStencil.depthWriteEnable;
    depthStencil.depthCompareOp = config.depthStencil.depthCompareOp;
    depthStencil.depthBoundsTestEnable = config.depthStencil.depthBoundsTestEnable;
    depthStencil.stencilTestEnable = config.depthStencil.stencilTestEnable;

    // 混合附件状态
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = config.colorBlend.colorWriteMask;
    colorBlendAttachment.blendEnable = config.colorBlend.blendEnable;
    colorBlendAttachment.srcColorBlendFactor = config.colorBlend.srcColorBlendFactor;
    colorBlendAttachment.dstColorBlendFactor = config.colorBlend.dstColorBlendFactor;
    colorBlendAttachment.colorBlendOp = config.colorBlend.colorBlendOp;
    colorBlendAttachment.srcAlphaBlendFactor = config.colorBlend.srcAlphaBlendFactor;
    colorBlendAttachment.dstAlphaBlendFactor = config.colorBlend.dstAlphaBlendFactor;
    colorBlendAttachment.alphaBlendOp = config.colorBlend.alphaBlendOp;

    // 混合状态
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    // 动态状态
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(config.dynamicStates.size());
    dynamicState.pDynamicStates = config.dynamicStates.data();

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = config.layout;
    pipelineInfo.renderPass = config.renderPass;
    pipelineInfo.layout = config.layout;
    pipelineInfo.renderPass = config.renderPass;
    pipelineInfo.subpass = config.subpass;

    VkPipeline pipeline;
    if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
        throw std::runtime_error("无法创建图形管线！");
    }

    return pipeline;
}

// PipelineManager::Builder 实现
PipelineManager::Builder::Builder(PipelineManager* manager) : manager(manager) {
    // 初始化默认配置
}

PipelineManager::Builder& PipelineManager::Builder::setShader(VkShaderStageFlagBits stage, const ShaderModule* shader) {
    if (!shader) return *this;

    switch (stage) {
    case VK_SHADER_STAGE_VERTEX_BIT:
        config.vertexShader = shader->getHandle();
        break;
    case VK_SHADER_STAGE_FRAGMENT_BIT:
        config.fragmentShader = shader->getHandle();
        break;
    case VK_SHADER_STAGE_GEOMETRY_BIT:
        config.geometryShader = shader->getHandle();
        break;
    case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
        config.tessControlShader = shader->getHandle();
        break;
    case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
        config.tessEvalShader = shader->getHandle();
        break;
    default:
        break;
    }

    return *this;
}

PipelineManager::Builder& PipelineManager::Builder::setVertexShader(const ShaderModule* shader) {
    return setShader(VK_SHADER_STAGE_VERTEX_BIT, shader);
}

PipelineManager::Builder& PipelineManager::Builder::setFragmentShader(const ShaderModule* shader) {
    return setShader(VK_SHADER_STAGE_FRAGMENT_BIT, shader);
}

PipelineManager::Builder& PipelineManager::Builder::setVertexInput(
    const std::vector<VkVertexInputBindingDescription>& bindings,
    const std::vector<VkVertexInputAttributeDescription>& attributes) {
    config.vertexInput.bindings = bindings;
    config.vertexInput.attributes = attributes;
    return *this;
}

PipelineManager::Builder& PipelineManager::Builder::setTopology(VkPrimitiveTopology topology) {
    config.topology = topology;
    return *this;
}

PipelineManager::Builder& PipelineManager::Builder::setCullMode(VkCullModeFlags cullMode) {
    config.rasterization.cullMode = cullMode;
    return *this;
}

PipelineManager::Builder& PipelineManager::Builder::setPolygonMode(VkPolygonMode polygonMode) {
    config.rasterization.polygonMode = polygonMode;
    return *this;
}

PipelineManager::Builder& PipelineManager::Builder::setDepthTest(bool enable) {
    config.depthStencil.depthTestEnable = enable;
    return *this;
}

PipelineManager::Builder& PipelineManager::Builder::setDepthWrite(bool enable) {
    config.depthStencil.depthWriteEnable = enable;
    return *this;
}

PipelineManager::Builder& PipelineManager::Builder::setDepthCompareOp(VkCompareOp compareOp) {
    config.depthStencil.depthCompareOp = compareOp;
    return *this;
}

PipelineManager::Builder& PipelineManager::Builder::setBlending(bool enable) {
    config.colorBlend.blendEnable = enable;
    return *this;
}

PipelineManager::Builder& PipelineManager::Builder::setBlendFactors(
    VkBlendFactor srcColor, VkBlendFactor dstColor,
    VkBlendFactor srcAlpha, VkBlendFactor dstAlpha) {
    config.colorBlend.srcColorBlendFactor = srcColor;
    config.colorBlend.dstColorBlendFactor = dstColor;
    config.colorBlend.srcAlphaBlendFactor = srcAlpha;
    config.colorBlend.dstAlphaBlendFactor = dstAlpha;
    return *this;
}

PipelineManager::Builder& PipelineManager::Builder::setPipelineLayout(VkPipelineLayout layout) {
    config.layout = layout;
    return *this;
}

PipelineManager::Builder& PipelineManager::Builder::setRenderPass(VkRenderPass renderPass, uint32_t subpass) {
    config.renderPass = renderPass;
    config.subpass = subpass;
    return *this;
}

VkPipeline PipelineManager::Builder::build() {
    return manager->getPipeline(config);
}

PipelineManager::Builder PipelineManager::createBuilder() {
    return Builder(this);
}
