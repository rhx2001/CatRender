#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include "MaterialViewer.h"

class Material {


private:

    // 材质参数数据
    struct MaterialBlock{
        alignas(16) glm::vec4 baseColorFactor = { 1.0,1.0,1.0,1.0 };
        alignas(4) float metallicFactor = 1.0f;
        alignas(4) float roughnessFactor = 1.0f;
        alignas(4) float occlusionStrength;
        alignas(16) char reserved[32]; // 预留给未来扩展
    } params;
    std::shared_ptr<MaterialViewer>(materialViewer);
    uint32_t materialId;
    uint32_t offSet;
    //std::unique_ptr<MaterialManager> materialManager;

    
    // 纹理引用

public:

    Material(uint32_t materialId, uint32_t offSet, const std::shared_ptr<MaterialViewer>& Viewer) :materialId(materialId), offSet(offSet), materialViewer(Viewer) {}
    void setBaseColor(const glm::vec4& color);
    void setMetallic(float value);
    void setRoughness(float value);
	uint32_t getOffset() const { return offSet; }

    void updateUniformData(void* mappedUniformBuffer);
	std::shared_ptr<MaterialViewer>& getMaterialViewer() { return materialViewer; }

};
