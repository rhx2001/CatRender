#pragma once
#include <unordered_map>
#include <vulkan/vulkan_core.h>

#include "Mesh.h"


#define VK_CHECK(f) { VkResult res = (f); if (res != VK_SUCCESS) { std::cerr << "Fatal: VkResult is \"" << res << "\" in " << __FILE__ << " at line " << __LINE__ << std::endl; abort(); } }

struct BufferInfo
{
	VkBuffer buffer;       // Vulkan »º³åÇø¾ä±ú
	VkDeviceMemory memory; // Vulkan ÄÚ´æ¾ä±ú
};

class BufferManager
{
public:

private:
	VkDevice device_;
	VkPhysicalDevice physicalDevice_;
	std::unordered_map<uint32_t, BufferInfo> bufferMap;
	uint32_t generateUniqueId();

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

public:
	BufferManager(VkDevice const& device, VkPhysicalDevice const& physicalDevice):device_(device),physicalDevice_(physicalDevice){}
	~BufferManager();

	BufferInfo getBuffer(uint32_t const id) const {return bufferMap.at(id);}

	void createVertexBuffer(Mesh*& mesh);

	void createIndexBuffer(Mesh*& mesh);

};
