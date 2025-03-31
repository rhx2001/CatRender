#pragma once
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

#include "MaterialViewer.h"
#include "ShaderManager.h"

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
    std::string name;
    //TODO:
    VkShaderModule vertexShader, fragmentShader;
    std::string vertexShaderPath, fragmentShaderPath;
    uint32_t materialId;
    uint32_t offSet;
    //std::unique_ptr<MaterialManager> materialManager;

    
    // 纹理引用

public:

    Material(uint32_t materialId, uint32_t offSet, const std::shared_ptr<MaterialViewer>& Viewer,  std::string name) :materialId(materialId), offSet(offSet), materialViewer(Viewer),name(name) {}
    void setBaseColor(const glm::vec4& color) { params.baseColorFactor = color; }
    void setMetallic(float value);
    void setRoughness(float value);
	uint32_t getOffset() const { return offSet; }

    glm::vec4 getBaseColor() const { return params.baseColorFactor; }
    float getMetallic() const { return params.metallicFactor; }
    float getRoughness() const { return params.roughnessFactor; }
    const std::string& getName() { return name; }

    void updateUniformData(void* mappedUniformBuffer) const;
	std::shared_ptr<MaterialViewer>& getMaterialViewer() { return materialViewer; }


    void InitMaterial(Material& material, ShaderManager& shaderManager) {
        this->vertexShader = shaderManager.LoadShader(this->vertexShaderPath);
        this->fragmentShader = shaderManager.LoadShader(this->fragmentShaderPath);
    }
};
