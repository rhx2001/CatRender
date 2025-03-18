#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include "MaterialViewer.h"

class Material {


private:
    uint32_t materialId;
    uint32_t offSet;
	//std::unique_ptr<MaterialManager> materialManager;
	std::shared_ptr<MaterialViewer>(materialViewer);
    // 材质参数数据
    struct {
        glm::vec4 baseColor = { 1.0,1.0,1.0,1.0 };
        float metallic = 0.0f;
        float roughness = 1.0f;
        float padding[2];
    } params;
    
    // 纹理引用

public:

    Material(uint32_t materialId, uint32_t offSet, const std::shared_ptr<MaterialViewer>& Viewer) :materialId(materialId), offSet(offSet), materialViewer(Viewer) {}
    void setBaseColor(const glm::vec4& color);
    void setMetallic(float value);
	uint32_t getOffset() const { return offSet; }


    // 更新统一缓冲区
    void updateUniformData();

	std::shared_ptr<MaterialViewer>& getMaterialViewer() { return materialViewer; }
};
