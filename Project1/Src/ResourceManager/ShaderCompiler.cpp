#include "ResourceManager/ShaderCompiler.h"
#include <shaderc/shaderc.hpp>
#include <fstream>
#include <iostream>

ShaderCompiler::ShaderCompiler(bool enableOptimization)
    : m_enableOptimization(enableOptimization) {
}

ShaderCompiler::~ShaderCompiler() {}

void ShaderCompiler::AddMacroDefinition(const std::string& name, const std::string& value) {
    m_macros[name] = value;
}

void ShaderCompiler::ClearMacroDefinitions() {
    m_macros.clear();
}

std::vector<uint32_t> ShaderCompiler::CompileGlslToSpirv(
    const std::string& glslCode,
    VkShaderStageFlagBits shaderStage,
    const std::string& shaderName)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    // 设置优化级别
    if (m_enableOptimization) {
        options.SetOptimizationLevel(shaderc_optimization_level_performance);
    }

    // 添加预定义宏
    for (const auto& macro : m_macros) {
        options.AddMacroDefinition(macro.first, macro.second);
    }

    // 转换 Vulkan Shader Stage 到 shaderc 类型
    shaderc_shader_kind kind;
    switch (shaderStage) {
    case VK_SHADER_STAGE_VERTEX_BIT:   kind = shaderc_vertex_shader; break;
    case VK_SHADER_STAGE_FRAGMENT_BIT: kind = shaderc_fragment_shader; break;
    case VK_SHADER_STAGE_COMPUTE_BIT:  kind = shaderc_compute_shader; break;
    default:
        std::cerr << "Unsupported shader stage!" << std::endl;
        return {};
    }

    // 编译 GLSL 到 SPIR-V
    shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(
        glslCode, kind, shaderName.empty() ? "unnamed_shader" : shaderName.c_str(), options);

    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        std::cerr << "Shader compilation failed:\n"
            << result.GetErrorMessage() << std::endl;
        return {};
    }

    return { result.cbegin(), result.cend() };
}

std::vector<uint32_t> ShaderCompiler::CompileShaderFile(
    const std::string& filePath,
    VkShaderStageFlagBits shaderStage)
{
    // 读取 GLSL 文件
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return {};
    }

    std::string glslCode(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );

    return CompileGlslToSpirv(glslCode, shaderStage, filePath);
}