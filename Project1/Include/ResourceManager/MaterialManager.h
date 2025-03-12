#pragma once
#include <unordered_map>
#include <vulkan/vulkan.h>
#include "Material.h"
#include "BufferManager.h"
class MaterialManager {
public:
    // ����һ���ض���ɫ���Ĳ���
    uint32_t createMaterial(uint32_t shaderId);

    // ���²��ʲ���
    void updateMaterialParams(uint32_t materialId, const void* data, size_t size);

    // ��ȡ������Ⱦ��MaterialViewer
    MaterialViewer* getMaterialViewer(uint32_t materialId);

private:
    std::unordered_map<uint32_t, Material> materials;
    BufferManager& bufferManager;
    //ShaderManager& shaderManager;
};