#include "MaterialManager.h"

#include <stb_image/stb_image.h>

MaterialManager::MaterialManager(BufferManager& bufferManager):bufferManager(bufferManager)
{
	TextureImageID = -1;
	MaterialID = -1;
	MaterialViewerID = -1;
}

void MaterialManager::loadTextureImage(std::vector<std::string> ImagePaths)
{
	for (auto path : ImagePaths)
	{
		uint32_t textureId = ImageTextureGenerator();
		int texWidth, texHeight, texChannels;//纹理图片的信息读取
		stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		VkDeviceSize imageSize = static_cast<VkDeviceSize>(texWidth) * texHeight * 4;
		mipLevels[textureId] = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		bufferManager.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
		bufferManager.copyFromStagingBuffer(stagingBufferMemory, pixels, imageSize);

		stbi_image_free(pixels);

		createImage(texWidth, texHeight, mipLevels, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			textureImage, textureImageMemory);
	}

}

uint32_t MaterialManager::ImageTextureGenerator()
{
	TextureImageID += 1;
	return TextureImageID;
}
