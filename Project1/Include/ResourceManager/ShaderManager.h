#pragma once
#include <string>
#include <unordered_map>
#include <vulkan/vulkan.h>
class ShaderManager
{
public:
	ShaderManager(VkDevice device):m_device(device){}
	~ShaderManager() = default;
	VkShaderModule LoadShader(const std::string& path);
private:
	VkDevice& m_device;
	std::unordered_map<std::string, VkShaderModule> m_shaderCache;
};
