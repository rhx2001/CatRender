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
    // ���ʲ�������
    struct {
        glm::vec4 baseColor;
        float metallic;
        float roughness;
        float padding[2];
    } params;
    
    // ��������

public:

    Material(uint32_t materialId, uint32_t offSet, const std::shared_ptr<MaterialViewer>& Viewer) :materialId(materialId), offSet(offSet), materialViewer(Viewer) {}
    void setBaseColor(const glm::vec4& color);
    void setMetallic(float value);


    // ����ͳһ������
    void updateUniformData();
};
