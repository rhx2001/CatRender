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
        alignas(16) char reserved[32]; // Ԥ����δ����չ
    };

    MaterialManager(BufferManager& bufferManager);
	~MaterialManager();
    // ����һ���ض���ɫ���Ĳ���
    void loadTextureImage(std::string path);

    //TODO:�������ܹ���̬����Ӳ���
    uint32_t createMaterial(uint32_t textureid);
    uint32_t createMaterial();//�������id��

    //TODO:�ò��ʵĲ����ܹ�����
    void updateMaterialParams(uint32_t materialId, const void* data, size_t size);


	// ����set����
    void setOffest(uint32_t offset) { BasicDynamicOffset = offset; }
    void setMaterialMeshMap(uint32_t materialID, uint32_t meshID);
	void setDescriptorPool(VkDescriptorPool descriptor_pool) { descriptorPool = descriptor_pool; }

    //����get����
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
    std::unordered_map<uint32_t, VkDeviceMemory> TextureImageMemorys;//����ͼ���ڴ棬���Һ�ÿһ��materialViewers��
	std::unordered_map<uint32_t, uint32_t> mipLevels;
    std::unordered_map<uint32_t, std::shared_ptr<Material>> materials;
    std::unordered_map<uint32_t, std::shared_ptr<MaterialViewer>> materialViewers;//�����ظ�����viewers
	std::unordered_map<uint32_t, std::vector<uint32_t>> MaterialBindMeshMap;

private:
    uint32_t ImageTextureIDGenerator();

    uint32_t ImageViewerIDGenerator();

    uint32_t MaterialIDGenerator();

    void createTextureSampler(uint32_t ImageViewerID);

	


    //ShaderManager& shaderManager;
};
