#include<ResourceManager/ShaderManager.h>

VkShaderModule ShaderManager::LoadShader(const std::string& path)
{
    // 1. 读取 GLSL 文件并编译为 SPIR-V（可用 glslangValidator 或 shaderc）
    std::vector<uint32_t> spirvCode = CompileGLSLToSPIRV(path);

    // 2. 创建 VkShaderModule
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = spirvCode.size() * sizeof(uint32_t);
    createInfo.pCode = spirvCode.data();

    VkShaderModule shaderModule;
    vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule);

    // 3. 缓存 Shader
    m_shaderCache[path] = shaderModule;
    return shaderModule;
}
