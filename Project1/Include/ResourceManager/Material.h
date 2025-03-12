#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <ResourceManager/MaterialManager.h>
class Material {
public:
    
    void setBaseColor(const glm::vec4& color);
    void setMetallic(float value);


    // ����ͳһ������
    void updateUniformData();

private:
    uint32_t materialId;
    uint32_t offSet;
	std::unique_ptr<MaterialManager> materialManager;
    
    // ���ʲ�������
    struct {
        glm::vec4 baseColor;
        float metallic;
        float roughness;
        float padding[2];
    } params;
    
    // ��������


};