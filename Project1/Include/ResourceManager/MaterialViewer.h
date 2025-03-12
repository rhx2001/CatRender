#pragma once
#include<vulkan/vulkan.h>
class MaterialViewer
{
private:
	VkSampler TextureSampler;
	VkImageView TextureImageView;

	uint32_t mipLevels;
	VkDescriptorSet descriptorSet;
public:
	MaterialViewer();
	~MaterialViewer();
	void createTextureImage(const std::string& path);
	void createTextureImageView();
	void createTextureSampler();
	void createDescriptorSet(VkDescriptorSetLayout descriptorSetLayout, VkDevice device);
	VkDescriptorSet getDescriptorSet() { return descriptorSet; }
	VkSampler getTextureSampler() { return TextureSampler; }
	VkImageView getTextureImageView() { return TextureImageView; }
	uint32_t getMipLevels() { return mipLevels; }
}