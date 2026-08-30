#pragma once
#include <ResourceManager/BaseMaterial.h>
#include "MaterialViewer.h"
#include "ShaderManager.h"

class PBRMaterial:public BaseMaterial {


private:
    // 材质参数数据
    struct MaterialBlock{
        alignas(16) glm::vec4 baseColorFactor = { 1.0,1.0,1.0,1.0 };
        alignas(4) float metallicFactor = 1.0f;
        alignas(4) float roughnessFactor = 1.0f;
        alignas(4) float occlusionStrength;
        alignas(16) char reserved[32]; // 预留给未来扩展
    } params;

    PipelineConfig pipelineConfig;

    std::vector < std::shared_ptr<MaterialViewer>>(materialViewer);
    std::string name;
	std::string RanderPassTag = "BaseRenderPass";

    //TODO:
    VkShaderModule vertexShader, fragmentShader;
    std::string vertexShaderPath, fragmentShaderPath;
    uint32_t materialId;
    uint32_t offSet;
    uint32_t pipelineID;
    //std::unique_ptr<MaterialManager> materialManager;

    
    // 纹理引用

public:

    PBRMaterial(uint32_t materialId, uint32_t offSet, const std::vector < std::shared_ptr<MaterialViewer>>& Viewers, std::string name);
    void setBaseColor(const glm::vec4& color) { params.baseColorFactor = color; }
    void setMetallic(float value);
    void setRoughness(float value);

    uint32_t getOffSet() const { return offSet; }
	void setOffSet(uint32_t offSet) { this->offSet = offSet; }

    glm::vec4 getBaseColor() const { return params.baseColorFactor; }
    float getMetallic() const { return params.metallicFactor; }
    float getRoughness() const { return params.roughnessFactor; }
    const std::string& getName() { return name; }

	uint32_t getPipelineID() const { return pipelineID; }
	std::string getRenderPassTage() override { return RanderPassTag; }

    //TODO:将pipelinemanaager传进来，然后根据材质的pipeline参数创建材质，并且给对应额pipelineID添加入Cache
    void setPipelineID(PipelineManager pipelineManager);

    void UpdateUniformDate(void* mappedUniformBuffer) const;
    std::vector < std::shared_ptr<MaterialViewer>>& getMaterialViewer() { return materialViewer; }

    void setMaterialViewer(uint32_t ViewerID, std::shared_ptr<MaterialViewer>& viewer) override {
		if (ViewerID >= materialViewer.size()) {
			materialViewer.resize(ViewerID + 1);
		}
	materialViewer[ViewerID] = viewer;
	}

	void addMaterialViewer(std::shared_ptr<MaterialViewer>& materialViewer) { this->materialViewer.push_back(materialViewer); }


};
