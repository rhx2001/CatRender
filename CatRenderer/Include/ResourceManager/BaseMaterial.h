#pragma once
#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <ResourceManager/PipelineManager.h>
#include "MaterialViewer.h"

class BaseMaterial
{
public:
	virtual ~BaseMaterial() {}
	virtual std::string getRenderPassTage() = 0;
	virtual void setBaseColor(const glm::vec4& color) = 0;
	virtual void setMetallic(float value) = 0;
	virtual void setRoughness(float value) = 0;
	virtual void UpdateUniformDate(void* mappedUniformBuffer) const = 0;
	virtual void setOffSet(uint32_t offSet) = 0;
	virtual uint32_t getOffSet() const = 0;
	virtual const std::string& getName() = 0;
	virtual uint32_t getPipelineID() const = 0;
	virtual void setPipelineID(PipelineManager pipelineManager) = 0;


	virtual std::vector<std::shared_ptr<MaterialViewer>>& getMaterialViewer() = 0;
	virtual void setMaterialViewer(uint32_t ViewerID, std::shared_ptr<MaterialViewer>& materialViewer) = 0;
	virtual void addMaterialViewer(std::shared_ptr<MaterialViewer>& materialViewer) = 0;

};
