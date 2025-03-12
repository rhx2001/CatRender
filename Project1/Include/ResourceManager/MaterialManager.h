#pragma once
#include <unordered_map>
#include <vulkan/vulkan.h>
#include "Material.h"
#include "BufferManager.h"
#include "MaterialViewer.h"

class MaterialManager {
public:
    // 创建一个特定着色器的材质
    uint32_t createMaterial(uint32_t textureid);
    uint32_t createMaterial();//不填材质id的

    // 更新材质参数
    void updateMaterialParams(uint32_t materialId, const void* data, size_t size);

    // 获取用于渲染的MaterialViewer
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
	std::unordered_map<uint32_t, VkDeviceMemory> TextureImageMemorys;//纹理图像内存，并且和每一个materialViewers绑定

    std::unordered_map<uint32_t, Material> materials;
	std::unordered_map<uint32_t, MaterialViewer> materialViewers;//方便重复利用viewers

    //ShaderManager& shaderManager;
};
