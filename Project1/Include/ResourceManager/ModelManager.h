#pragma once
#include <memory>
#include <ResourceManager/ModelInstance.h>
#include <unordered_map>
class ModelManager
{
public:


private:
	std::unordered_map<uint32_t, std::shared_ptr<Mesh>> bindMesh;
	std::unordered_map<uint32_t, std::shared_ptr<modelInstance>> bindModels;
	std::unordered_map<uint32_t, std::vector<uint32_t>> ModelBindMeshMap;

	uint32_t modelId = 0;
	uint32_t Basic_DynamicUniformBufferOffset = 0;

public:
	ModelManager();
	~ModelManager();

	void LoadMeshs(const std::vector<std::string>& paths);
	std::unordered_map<uint32_t, std::shared_ptr<Mesh>>* getMeshs() { return &bindMesh; }
	std::shared_ptr<Mesh> getMesh(uint32_t meshId) { return bindMesh[meshId]; }

	void LoadMaterials();
	//创建model实列并且加载mesh
	void createModelInstance();
	void createModelInstance(std::string name);
	void createModelInstance(uint32_t meshId);
	void createModelInstance(std::string name, uint32_t meshId);
	void createModelInstance(std::string name, glm::mat4 transM);
	void createModelInstance(glm::mat4 transM, uint32_t meshId);
	void createModelInstance(std::string name, glm::mat4 transM, uint32_t meshId);
	void createModelInstance(std::string name, glm::mat4 transM, uint32_t meshId, uint32_t materialId);

	void modelBindMesh(uint32_t meshId, uint32_t modelId);
	std::unordered_map<uint32_t, std::vector<uint32_t>>* getModelBindMeshMap() { return &ModelBindMeshMap; }
	std::shared_ptr<modelInstance> getModelInstance(uint32_t modelId) { return bindModels[modelId]; }

	void setOffest(uint32_t offset) { Basic_DynamicUniformBufferOffset = offset; }

	//TODO: 将和同一个mesh的model绑定在一起，然后减少切换vertexbuffer以及indexbuffer的次数。


private:
	uint32_t generateModelId();

	void bindModelMeshMap(uint32_t modelId, uint32_t meshId);
};
