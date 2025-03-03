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
	//VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT��ʾ���buffer�����ݿ��Ա�cpu��ȡ

	//�����ݿ�����buffer��
	void* data;
	vkMapMemory(device_, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, mesh->getVertices().data(), (size_t)bufferSize);
	vkUnmapMemory(device_, stagingBufferMemory);

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	//����һ��device_ local��buffer��ֻ��gpu�ܷ��ʵ�
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	//д����ϣ�ɾ��stagingBuffer
	vkDestroyBuffer(device_, stagingBuffer, nullptr);
	vkFreeMemory(device_, stagingBufferMemory, nullptr);

	mesh->setVertexBufferId(id);
	bufferMap[id] = BufferInfo{ vertexBuffer, vertexBufferMemory };
}

void BufferManager::createIndexBuffer(Mesh*& mesh)
{
	//ͬvertexbuffer
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
	//1. ��������������
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO; //�ṹ������
	bufferInfo.size = size; //�������Ĵ�С(�ֽ���)
	bufferInfo.usage = usage; //����������;(�綥�㻺���������仺������)
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; //�������Ĺ���ģʽ(��ռģʽ)

	//���� Vulkan API ����������
	VK_CHECK(vkCreateBuffer(device_, &bufferInfo, nullptr, &buffer))

		//2.��ѯ���������ڴ�����
		VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device_, buffer, &memRequirements);

	//3. �����ڴ�
	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO; // �ṹ������
	allocInfo.allocationSize = memRequirements.size; // ��Ҫ������ڴ��С
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties); // �ҵ����ʵ��ڴ���������

	//���� Vulkan API �����ڴ�
	VK_CHECK(vkAllocateMemory(device_, &allocInfo, nullptr, &bufferMemory))

	//4. ���ڴ�󶨵�������
	vkBindBufferMemory(device_, buffer, bufferMemory, 0); //��������ڴ�󶨵�����������
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


