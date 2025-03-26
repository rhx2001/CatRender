#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>
#include "ResourceManager/MaterialManager.h"

#include <iostream>
#include <ostream>


MaterialManager::MaterialManager(BufferManager& bufferManager):bufferManager(bufferManager)
{
	TextureImageID = -1;
	MaterialID = -1;
	MaterialViewerID = -1;
}

MaterialManager::~MaterialManager()
{
	for (auto& pair : TextureImages)
	{
		vkDestroyImage(bufferManager.getDevice(), pair.second, nullptr);
	}
	for (auto& pair : TextureImageMemorys)
	{
		vkFreeMemory(bufferManager.getDevice(), pair.second, nullptr);
	}
	for (auto& pair : materialViewers)
	{
		vkDestroyImageView(bufferManager.getDevice(), pair.second->getTextureImageView(), nullptr);
		vkDestroySampler(bufferManager.getDevice(), pair.second->getTextureSampler(), nullptr);
		//vkFreeDescriptorSets(bufferManager.getDevice(), descriptorPool, pair.second->getDescriptorSets().size(), pair.second->getDescriptorSets().data());

	}


	for (auto& pair : materials)
	{
		//vkDestroyDescriptorSetLayout(bufferManager.getDevice(), pair.second->getDescriptorSetLayout(), nullptr);
	}
	//vkDestroyDescriptorPool(bufferManager.getDevice(), descriptorPool, nullptr);
}

void MaterialManager::loadTextureImage(std::string path)
{
	uint32_t textureId = ImageTextureIDGenerator();
	int texWidth, texHeight, texChannels;//纹理图片的信息读取
	stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = static_cast<VkDeviceSize>(texWidth) * texHeight * 4;
	uint32_t mipLevel = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	bufferManager.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	bufferManager.copyFromStagingBuffer(stagingBufferMemory, pixels, static_cast<size_t>(imageSize), 0, 0);

	stbi_image_free(pixels);

	bufferManager.createImage(texWidth, texHeight, mipLevel, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		TextureImages[textureId], TextureImageMemorys[textureId]);

	bufferManager.transitionImageLayout(TextureImages[textureId], VK_FORMAT_R8G8B8A8_SRGB,
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevel);
	bufferManager.copyBufferToImage(stagingBuffer, TextureImages[textureId], static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	//
	//transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels);

	bufferManager.generateMipmaps(TextureImages[textureId], VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevel);//生成贴图对应的mipmap

	vkDestroyBuffer(bufferManager.getDevice(), stagingBuffer, nullptr);
	vkFreeMemory(bufferManager.getDevice(), stagingBufferMemory, nullptr);

	VkImageView Imageview = bufferManager.createImageView(TextureImages[textureId], VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevel);

	uint32_t ImageViewerID = ImageViewerIDGenerator();

	materialViewers[ImageViewerID] = std::make_shared < MaterialViewer >(Imageview, mipLevel);
	createTextureSampler(ImageViewerID);
	uint32_t materialId = MaterialIDGenerator();
	materials[textureId] = std::make_shared< Material>(materialId, BasicDynamicOffset * materialId, materialViewers[ImageViewerID]);
}


void MaterialManager::setMaterialMeshMap(uint32_t materialID, uint32_t meshID)
{
	if (MaterialBindMeshMap.find(meshID) == MaterialBindMeshMap.end())
	{
		MaterialBindMeshMap[materialID] = std::vector<uint32_t>{ meshID };
	}
	else
	{
		MaterialBindMeshMap[materialID].push_back(meshID);
	}
}

uint32_t MaterialManager::ImageTextureIDGenerator()
{
	TextureImageID += 1;
	return TextureImageID;
}

uint32_t MaterialManager::ImageViewerIDGenerator()
{
	MaterialViewerID += 1;
	return MaterialViewerID;
}

uint32_t MaterialManager::MaterialIDGenerator()
{
	MaterialID += 1;
	return MaterialViewerID;
}

void MaterialManager::createTextureSampler(uint32_t ImageViewerID)
{
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;//放大过滤器
	samplerInfo.minFilter = VK_FILTER_LINEAR;//缩小过滤器
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;//U轴的寻址模式
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;//V轴的寻址模式
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;//W轴的寻址模式
	samplerInfo.anisotropyEnable = VK_TRUE;//各向异性过滤

	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;//边界颜色
	samplerInfo.unnormalizedCoordinates = VK_FALSE;//不归一化坐标轴
	samplerInfo.compareEnable = VK_FALSE;//禁用比较功能（VK_FALSE）。比较功能用于深度纹理采样时，比较采样值和参考值（通常用于阴影贴图等）。
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS; //设置比较操作为始终通过
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR; //设置多级渐远纹理的过滤模式为线性过滤
	samplerInfo.maxLod = VK_LOD_CLAMP_NONE; // 不限制最大细节层级

	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(bufferManager.getPhysicalDevice(), &properties);//根据设备来设置采样器的一些数值
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

	VkSampler textureSampler;

	VK_CHECK(vkCreateSampler(bufferManager.getDevice(), &samplerInfo, nullptr, &textureSampler))

	materialViewers[ImageViewerID]->createTextureSampler(textureSampler);

}
