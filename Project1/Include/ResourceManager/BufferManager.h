/*
 * 这是buffermanager，用于管理各种buffer的创建和删除，目前就只做了vertexbuffer以及indexbuffer
 */
#pragma once
#include <unordered_map>
#include <vulkan/vulkan_core.h>
#include <stdexcept>
#include "Mesh.h"


#define VK_CHECK(f) { VkResult res = (f); if (res != VK_SUCCESS) { std::cerr << "Fatal: VkResult is \"" << res << "\" in " << __FILE__ << " at line " << __LINE__ << std::endl; abort(); } }

struct BufferInfo
{
	VkBuffer buffer;       // Vulkan 缓冲区句柄
	VkDeviceMemory memory; // Vulkan 内存句柄
};

class BufferManager
{
public:
	
private:
	VkDevice& device_;
	VkPhysicalDevice& physicalDevice_;
	VkCommandPool& commandPool_;
	VkQueue& graphicsQueue_;
	std::unordered_map<uint32_t, BufferInfo> bufferMap;

	uint32_t BufferID = -1;

	uint32_t generateUniqueId();

	VkCommandBuffer beginSingleTimeCommands() const;

	void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

	static bool hasStencilComponent(VkFormat format) { return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT; }

public:
	BufferManager(VkDevice& device, VkPhysicalDevice& physicalDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue):
	device_(device),physicalDevice_(physicalDevice), commandPool_(commandPool), graphicsQueue_(graphicsQueue){}

	~BufferManager();

	VkDevice getDevice() const { return device_; }

	VkPhysicalDevice getPhysicalDevice() const { return physicalDevice_; }

	BufferInfo getBuffer(uint32_t const id) const {return bufferMap.at(id);}

	void createVertexBuffer(Mesh* mesh);

	void createIndexBuffer(Mesh* mesh);

	void createImage (uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format,
		VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) const;

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	template<typename T>
	void copyFromStagingBuffer(VkDeviceMemory& stageBufferMemory, const T& data, uint32_t size, uint32_t offset, uint32_t flags);

	void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format,
		VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) const;

	void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels) const;

	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) const;


};

template<typename T>
inline void BufferManager::copyFromStagingBuffer(VkDeviceMemory& stageBufferMemory, const T& srcData, uint32_t size, uint32_t offset, uint32_t flags)
{
	void* mapData;
	VkResult result = vkMapMemory(device_, stageBufferMemory, offset, size, flags, &mapData);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to map memory");
	}
	memcpy(mapData, srcData, size);
	vkUnmapMemory(device_, stageBufferMemory);
}
