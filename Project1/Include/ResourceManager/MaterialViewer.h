#pragma once
#include<vulkan/vulkan.h>
class MaterialViewer
{
private:
	VkSampler TextureSampler = VK_NULL_HANDLE;
	VkImageView TextureImageView;

	uint32_t mipLevels;
	std::vector<VkDescriptorSet> descriptorSets;
public:
	MaterialViewer();
	MaterialViewer(VkImageView TextureImageView, uint32_t mipLevels, uint32_t numOfFrame) :
		TextureImageView(TextureImageView), mipLevels(mipLevels) {
		descriptorSets.resize(numOfFrame);
	}
	~MaterialViewer();
	void createTextureImage(const std::string& path);
	void createTextureImageView();

	void createTextureSampler(VkSampler TextureSampler) :TextureSampler(TextureSampler) {}

	void setDescriptorSet(uint32_t FRAME_IN_FLIGHT, VkDescriptorSet descriptorSet);

	VkDescriptorSet getDescriptorSet(uint32_t FRAME_IN_FLIGHT) const { return descriptorSets[FRAME_IN_FLIGHT]; }
	VkSampler getTextureSampler() const { return TextureSampler; }
	VkImageView getTextureImageView() const { return TextureImageView; }
	uint32_t getMipLevels() const { return mipLevels; }
};