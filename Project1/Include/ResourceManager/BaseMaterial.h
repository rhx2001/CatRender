#pragma once
#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "MaterialViewer.h"

class BaseMaterial
{
public:
	virtual ~BaseMaterial() {}
	virtual getRenderPassTage() = 0;
	virtual void setBaseColor(const glm::vec4& color) = 0;
	virtual void setMetallic(float value) = 0;
	virtual void setRoughness(float value) = 0;
	virtual void UpdateUniformDate(void* mappedUniformBuffer) = 0;
	virtual void setOffSet(uint32_t offSet) = 0;
	virtual uint32_t getOffSet() const = 0;
	virtual std::string getName() = 0;
	virtual uint32_t getPipelineID() = 0;
	virtual void setPipelineID(uint32_t pipelineID) = 0;


	virtual std::vector<std::shared_ptr<MaterialViewer>>& getMaterialViewer() = 0;
	virtual void setMaterialViewer(std::shared_ptr<MaterialViewer>& materialViewer) = 0;
};
