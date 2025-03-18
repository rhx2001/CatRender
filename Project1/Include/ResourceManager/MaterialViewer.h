#pragma once
#include <string>
#include <vector>
#include<vulkan/vulkan.h>
class MaterialViewer
{
private:
	VkSampler t_TextureSampler = VK_NULL_HANDLE;
	VkImageView TextureImageView;

	uint32_t mipLevels;
	std::vector<VkDescriptorSet> m_descriptorSets;
public:
	MaterialViewer();
	MaterialViewer(VkImageView TextureImageView, uint32_t mipLevels) :
		TextureImageView(TextureImageView), mipLevels(mipLevels){}
	void createTextureImage(const std::string& path);
	void createTextureImageView();

	void createTextureSampler(VkSampler TextureSampler) { t_TextureSampler = TextureSampler; }

	void setDescriptorSetByFrame(uint32_t FRAME_IN_FLIGHT, VkDescriptorSet descriptorSet);

	void setDescriptorSets(const std::vector<VkDescriptorSet>& descriptorSets) { m_descriptorSets = descriptorSets; }

	VkDescriptorSet& getDescriptorSet(uint32_t FRAME_IN_FLIGHT) { return m_descriptorSets[FRAME_IN_FLIGHT]; }
	std::vector<VkDescriptorSet>& getDescriptorSets() { return m_descriptorSets; }
	VkSampler getTextureSampler() const { return t_TextureSampler; }
	VkImageView getTextureImageView() const { return TextureImageView; }
	uint32_t getMipLevels() const { return mipLevels; }
};