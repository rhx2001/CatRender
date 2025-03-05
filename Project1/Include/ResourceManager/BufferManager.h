/*
 * ����buffermanager�����ڹ������buffer�Ĵ�����ɾ����Ŀǰ��ֻ����vertexbuffer�Լ�indexbuffer
 */
#pragma once
#include <unordered_map>
#include <vulkan/vulkan_core.h>

#include "Mesh.h"


#define VK_CHECK(f) { VkResult res = (f); if (res != VK_SUCCESS) { std::cerr << "Fatal: VkResult is \"" << res << "\" in " << __FILE__ << " at line " << __LINE__ << std::endl; abort(); } }

struct BufferInfo
{
	VkBuffer buffer;       // Vulkan ���������
	VkDeviceMemory memory; // Vulkan �ڴ���
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

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	VkCommandBuffer beginSingleTimeCommands() const;

	void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

public:
	BufferManager(VkDevice& device, VkPhysicalDevice& physicalDevice, VkCommandPool& commandPool, VkQueue& graphicsQueue):
	device_(device),physicalDevice_(physicalDevice), commandPool_(commandPool), graphicsQueue_(graphicsQueue){}

	~BufferManager();

	BufferInfo getBuffer(uint32_t const id) const {return bufferMap.at(id);}

	void createVertexBuffer(Mesh* mesh);

	void createIndexBuffer(Mesh* mesh);



};
