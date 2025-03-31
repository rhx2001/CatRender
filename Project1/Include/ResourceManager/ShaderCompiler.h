#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <vulkan/vulkan.h>

class ShaderCompiler {
public:
    // 初始化时指定是否启用优化
    explicit ShaderCompiler(bool enableOptimization = false);
    ~ShaderCompiler();

    // 编译 GLSL 代码到 SPIR-V
    std::vector<uint32_t> CompileGlslToSpirv(
        const std::string& glslCode,
        VkShaderStageFlagBits shaderStage,
        const std::string& shaderName = "");

    // 从文件加载并编译
    std::vector<uint32_t> CompileShaderFile(
        const std::string& filePath,
        VkShaderStageFlagBits shaderStage);

    // 添加预定义宏（如 #define USE_NORMAL_MAP 1）
    void AddMacroDefinition(const std::string& name, const std::string& value = "");

    // 清除所有宏定义
    void ClearMacroDefinitions();

private:
    bool m_enableOptimization;
    std::unordered_map<std::string, std::string> m_macros;
};