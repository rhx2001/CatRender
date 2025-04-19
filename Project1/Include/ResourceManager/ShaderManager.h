#pragma once
//#include <memory>
//#include <string>
//#include <unordered_map>
//#include <vulkan/vulkan.h>
//#include <ResourceManager/ShaderCompiler.h>
//
//class ShaderManager
//{
//public:
//	ShaderManager(VkDevice& device) :m_device(device), shaderCompiler(std::make_unique<ShaderCompiler>()) {}
//	~ShaderManager() = default;
//	VkShaderModule LoadShader(const std::string& path, VkShaderStageFlagBits stage);
//
//	std::unordered_map<std::string, VkShaderModule>& getShaderModules() { return m_shaderCache; }
//private:
//	VkDevice& m_device;
//	std::unique_ptr<ShaderCompiler> shaderCompiler;
//	std::unordered_map<std::string, VkShaderModule> m_shaderCache;
//};
