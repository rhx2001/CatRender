#include "ResourceManager/BufferManager.h"
#include <iostream>
#include <ostream>

BufferManager::~BufferManager() {
	for (auto& pair : bufferMap) {
		// 销毁 VkBuffer
		if (pair.second.buffer != VK_NULL_HANDLE) {
			vkDestroyBuffer(device_, pair.second.buffer, nullptr);
		}
		// 释放 VkDeviceMemory
		if (pair.second.memory != VK_NULL_HANDLE) {
			vkFreeMemory(device_, pair.second.memory, nullptr);
		}
	}
	// bufferMap 会在离开作用域时自动清理
}

void BufferManager::createVertexBuffer(Mesh* mesh)
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

void BufferManager::createIndexBuffer(Mesh* mesh)
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

void BufferManager::createImage (uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, 
	VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) const
{
	{
		//创建一个image
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = mipLevels;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;


		VK_CHECK(vkCreateImage(device_, &imageInfo, nullptr, &image))
		//为image分配内存
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device_, image, &memRequirements);

		//分配内存
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		VK_CHECK(vkAllocateMemory(device_, &allocInfo, nullptr, &imageMemory))
		VK_CHECK(vkBindImageMemory(device_, image, imageMemory, 0))

	}
}

uint32_t BufferManager::generateUniqueId()
{
	return ++BufferID;
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
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer);
}


VkCommandBuffer BufferManager::beginSingleTimeCommands() const
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool_;
	allocInfo.commandBufferCount = 1;
	//分配一个commandbuffer
	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device_, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void BufferManager::endSingleTimeCommands(VkCommandBuffer commandBuffer) const {
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue_);

	vkFreeCommandBuffers(device_, commandPool_, 1, &commandBuffer);
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


