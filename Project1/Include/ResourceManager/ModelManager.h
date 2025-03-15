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

	uint32_t modelID_ = 0;
	uint32_t Basic_DynamicUniformBufferOffset = 0;

public:
	//ModelManager();
	//~ModelManager();

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

	void modelBindMesh(uint32_t meshID, uint32_t modelID);
	std::unordered_map<uint32_t, std::vector<uint32_t>>* getModelBindMeshMap() { return &ModelBindMeshMap; }
	std::vector<uint32_t>& getModelBindMesh(uint32_t meshID) { return ModelBindMeshMap.at(meshID); }
	std::shared_ptr<modelInstance>& getModelInstanceByID(uint32_t modelID) { return bindModels[modelID]; }
	std::unordered_map<uint32_t, std::shared_ptr<modelInstance>>& getModelInstances() { return bindModels; }

	void setOffest(uint32_t offset) { Basic_DynamicUniformBufferOffset = offset; }
	uint32_t getOffeset() { return Basic_DynamicUniformBufferOffset; }


private:
	uint32_t generateModelId();

	void bindModelMeshMap(uint32_t modelId, uint32_t meshId);
};
