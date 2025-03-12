#pragma once
#include <unordered_map>
#include <vulkan/vulkan.h>
#include "Material.h"
#include "BufferManager.h"
#include "MaterialViewer.h"

class MaterialManager {
public:
    // ����һ���ض���ɫ���Ĳ���
    uint32_t createMaterial(uint32_t textureid);
    uint32_t createMaterial();//�������id��

    // ���²��ʲ���
    void updateMaterialParams(uint32_t materialId, const void* data, size_t size);

    // ��ȡ������Ⱦ��MaterialViewer
    MaterialViewer* getMaterialViewer(uint32_t materialId);


    void bind(VkCommandBuffer cmd, VkPipelineLayout layout);

private:
	VkDescriptorPool descriptorPool;
    BufferManager& bufferManager;
	uint32_t TextureImageID;
	uint32_t MaterialID;
	uint32_t MaterialViewerID;


private:

	std::unordered_map<uint32_t, VkImage> TextureImages;
	std::unordered_map<uint32_t, VkDeviceMemory> TextureImageMemorys;//����ͼ���ڴ棬���Һ�ÿһ��materialViewers��

    std::unordered_map<uint32_t, Material> materials;
	std::unordered_map<uint32_t, MaterialViewer> materialViewers;//�����ظ�����viewers

    //ShaderManager& shaderManager;
};
