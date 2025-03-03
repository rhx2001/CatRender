#include "ResourceManager/BufferManager.h"
#include <iostream>
#include <ostream>

BufferManager::~BufferManager()
{
}

void BufferManager::createVertexBuffer(Mesh*& mesh)
{
	uint32_t id = generateUniqueId();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VkDeviceSize bufferSize = sizeof(mesh->getVertices()[0]) * mesh->getVertices().size();
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
	//VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT表示这个buffer的数据可以被cpu读取

	//将数据拷贝到buffer中
	void* data;
	vkMapMemory(device_, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, mesh->getVertices().data(), (size_t)bufferSize);
	vkUnmapMemory(device_, stagingBufferMemory);

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	//创建一个device_ local的buffer，只有gpu能访问到
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	//写入完毕，删除stagingBuffer
	vkDestroyBuffer(device_, stagingBuffer, nullptr);
	vkFreeMemory(device_, stagingBufferMemory, nullptr);

	mesh->setVertexBufferId(id);
	bufferMap[id] = BufferInfo{ vertexBuffer, vertexBufferMemory };
}

void BufferManager::createIndexBuffer(Mesh*& mesh)
{
	//同vertexbuffer
	uint32_t id = generateUniqueId();
	VkDeviceSize bufferSize = sizeof(mesh->getIndices()[0]) * mesh->getIndices().size();
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
	void* data;
	vkMapMemory(device_, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, mesh->getIndices().data(), (size_t)bufferSize);
	vkUnmapMemory(device_, stagingBufferMemory);

	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	copyBuffer(stagingBuffer, indexBuffer, bufferSize);
	mesh->setIndexBufferId(id);

	vkDestroyBuffer(device_, stagingBuffer, nullptr);
	vkFreeMemory(device_, stagingBufferMemory, nullptr);

	bufferMap[id] = BufferInfo{ indexBuffer, indexBufferMemory };
}

void BufferManager::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	//1. 创建缓冲区对象
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO; //结构体类型
	bufferInfo.size = size; //缓冲区的大小(字节数)
	bufferInfo.usage = usage; //缓冲区的用途(如顶点缓冲区、传输缓冲区等)
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; //缓冲区的共享模式(独占模式)

	//调用 Vulkan API 创建缓冲区
	VK_CHECK(vkCreateBuffer(device_, &bufferInfo, nullptr, &buffer))

		//2.查询缓冲区的内存需求
		VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device_, buffer, &memRequirements);

	//3. 分配内存
	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO; // 结构体类型
	allocInfo.allocationSize = memRequirements.size; // 需要分配的内存大小
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties); // 找到合适的内存类型索引

	//调用 Vulkan API 分配内存
	VK_CHECK(vkAllocateMemory(device_, &allocInfo, nullptr, &bufferMemory))

	//4. 将内存绑定到缓冲区
	vkBindBufferMemory(device_, buffer, bufferMemory, 0); //将分配的内存绑定到缓冲区对象
}

void BufferManager::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{

}

uint32_t BufferManager::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice_, &memProperties);
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}


