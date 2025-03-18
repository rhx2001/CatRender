#pragma once
#include <unordered_map>
#include <vulkan/vulkan.h>
#include "ResourceManager/Material.h"
#include "ResourceManager/BufferManager.h"
#include "ResourceManager/MaterialViewer.h"

class MaterialManager {

public:
    struct MaterialBlock {
        alignas(16) glm::vec4 baseColorFactor;
        alignas(4) float metallicFactor;
        alignas(4) float roughnessFactor;
        alignas(4) float occlusionStrength;
        alignas(16) char reserved[32]; // 预留给未来扩展
    };

    MaterialManager(BufferManager& bufferManager);
	~MaterialManager();
    // 创建一个特定着色器的材质
    void loadTextureImage(std::string path);

    //TODO:让引擎能够动态的添加材质
    uint32_t createMaterial(uint32_t textureid);
    uint32_t createMaterial();//不填材质id的

    //TODO:让材质的参数能够更新
    void updateMaterialParams(uint32_t materialId, const void* data, size_t size);


	// 各种set函数
    void setOffest(uint32_t offset) { BasicDynamicOffset = offset; }
    void setMaterialMeshMap(uint32_t materialID, uint32_t meshID);
	void setDescriptorPool(VkDescriptorPool descriptor_pool) { descriptorPool = descriptor_pool; }

    //各种get函数
    uint32_t getOffeset() const { return BasicDynamicOffset; }
    std::unordered_map<uint32_t, std::shared_ptr<Material>> getMaterials() { return materials; }
	std::shared_ptr<Material>& getMaterial(uint32_t materialID) { return materials[materialID]; }
    std::unordered_map<uint32_t, std::vector<uint32_t>>& getMaterialMeshMap() { return MaterialBindMeshMap; }
    std::shared_ptr < MaterialViewer> getMaterialViewer(const uint32_t materialId) { return  materialViewers[materialId]; }
    std::unordered_map<uint32_t, std::shared_ptr<MaterialViewer>> getMaterialViewers() { return materialViewers; }

private:
	VkDescriptorPool descriptorPool;
    BufferManager bufferManager;
	uint32_t TextureImageID = -1;
	uint32_t MaterialID;
	uint32_t MaterialViewerID = -1;
    uint32_t BasicDynamicOffset = 0;

    std::unordered_map<uint32_t, VkImage> TextureImages;
    std::unordered_map<uint32_t, VkDeviceMemory> TextureImageMemorys;//纹理图像内存，并且和每一个materialViewers绑定
	std::unordered_map<uint32_t, uint32_t> mipLevels;
    std::unordered_map<uint32_t, std::shared_ptr<Material>> materials;
    std::unordered_map<uint32_t, std::shared_ptr<MaterialViewer>> materialViewers;//方便重复利用viewers
	std::unordered_map<uint32_t, std::vector<uint32_t>> MaterialBindMeshMap;

private:
    uint32_t ImageTextureIDGenerator();

    uint32_t ImageViewerIDGenerator();

    uint32_t MaterialIDGenerator();

    void createTextureSampler(uint32_t ImageViewerID);

	


    //ShaderManager& shaderManager;
};
