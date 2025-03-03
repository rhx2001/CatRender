#pragma once
#include <memory>
#include <ResourceManager/ModelInstance.h>
#include <unordered_map>
class ModelManager
{
public:


private:
	std::unordered_map<uint32_t, std::shared_ptr<Mesh>> bindMesh;
	std::unordered_map<std::string, std::shared_ptr<modelInstance>> bindModels;
	uint32_t modelId = 0;
	uint32_t Basic_DynamicUniformBufferOffset = 0;

public:
	ModelManager();
	~ModelManager();

	void LoadMeshs(const std::vector<std::string>& paths);
	void LoadMaterials();
	//创建model实列并且加载mesh
	void createModelInstance();
	void createModelInstance(std::string name);
	void createModelInstance(std::string name, glm::mat4 transM);
	void createModelInstance(std::string name, glm::mat4 transM, uint32_t meshId);
	void createModelInstance(std::string name, glm::mat4 transM, uint32_t meshId, uint32_t materialId);

	void setOffest(uint32_t offset) { Basic_DynamicUniformBufferOffset = offset; }

private:
	uint32_t generateId();
};
