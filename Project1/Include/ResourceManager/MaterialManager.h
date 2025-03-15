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


    // ����һ���ض���ɫ���Ĳ���
    void loadTextureImage(std::string path);

    uint32_t createMaterial(uint32_t textureid);
    uint32_t createMaterial();//�������id��

    // ���²��ʲ���
    void updateMaterialParams(uint32_t materialId, const void* data, size_t size);

    // ��ȡ������Ⱦ��MaterialViewer
    std::shared_ptr < MaterialViewer> getMaterialViewer (const uint32_t materialId) { return  materialViewers[materialId]; }

	std::unordered_map<uint32_t, std::shared_ptr<MaterialViewer>> getMaterialViewers() { return materialViewers; }


    void bind(VkCommandBuffer cmd, VkPipelineLayout layout);


    void setOffest(uint32_t offset) { BasicDynamicOffset = offset; }
    uint32_t getOffeset() const { return BasicDynamicOffset; }

private:
	VkDescriptorPool descriptorPool;
    BufferManager& bufferManager;
	uint32_t TextureImageID = -1;
	uint32_t MaterialID;
	uint32_t MaterialViewerID = -1;
    uint32_t BasicDynamicOffset = 0;

    std::unordered_map<uint32_t, VkImage> TextureImages;
    std::unordered_map<uint32_t, VkDeviceMemory> TextureImageMemorys;//����ͼ���ڴ棬���Һ�ÿһ��materialViewers��
	std::unordered_map<uint32_t, uint32_t> mipLevels;

    std::unordered_map<uint32_t, std::shared_ptr< Material>> materials;
    std::unordered_map<uint32_t, std::shared_ptr<MaterialViewer>> materialViewers;//�����ظ�����viewers

private:
    uint32_t ImageTextureIDGenerator();

    uint32_t ImageViewerIDGenerator();

    uint32_t MaterialIDGenerator();

    void createTextureSampler(uint32_t ImageViewerID);



    //ShaderManager& shaderManager;
};
