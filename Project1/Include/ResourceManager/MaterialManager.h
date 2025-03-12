#pragma once
#include <unordered_map>
#include <vulkan/vulkan.h>
#include "Material.h"
#include "BufferManager.h"
class MaterialManager {
public:
    // 创建一个特定着色器的材质
    uint32_t createMaterial(uint32_t shaderId);

    // 更新材质参数
    void updateMaterialParams(uint32_t materialId, const void* data, size_t size);

    // 获取用于渲染的MaterialViewer
    MaterialViewer* getMaterialViewer(uint32_t materialId);

private:
    std::unordered_map<uint32_t, Material> materials;
    BufferManager& bufferManager;
    //ShaderManager& shaderManager;
};