#pragma once
#include <memory>
#include <ResourceManager/ModelInstance.h>
#include <unordered_map>
#include <ResourceManager/MaterialManager.h>
class ModelManager
{
public:
	std::shared_ptr<MaterialManager> m_materialManager;

private:
	std::unordered_map<uint32_t, std::shared_ptr<Mesh>> bindMesh;
	std::unordered_map<uint32_t, std::shared_ptr<modelInstance>> bindModels;
	std::unordered_map<uint32_t, std::vector<uint32_t>> ModelBindMeshMap;
	std::unordered_map<uint32_t, uint32_t> MeshBindModelMap;
	std::unordered_map<std::string, uint32_t> ModelInstanceNameMap;

	uint32_t modelID_ = 0;
	uint32_t Basic_DynamicUniformBufferOffset = 0;

public:
	ModelManager(const std::shared_ptr<MaterialManager>& materialManager):m_materialManager(materialManager){}
	//~ModelManager();

	void LoadMeshs(const std::vector<std::string>& paths);


	void LoadMaterials();
	//创建model实列并且加载mesh
	//不带名字的
	void createModelInstance();

	void createModelInstance(uint32_t meshId);
	void createModelInstance(glm::mat4 transM, uint32_t meshId);
	void createModelInstance(uint32_t meshId, uint32_t materialId);

	//带名字的
	void createModelInstance(std::string name);
	void createModelInstance(std::string name, uint32_t meshId);
	void createModelInstance(std::string name, glm::mat4 transM);
	void createModelInstance(std::string name, glm::mat4 transM, uint32_t meshId);
	void createModelInstance(std::string name, glm::mat4 transM, uint32_t meshId, uint32_t materialId);

	void modelBindMesh(uint32_t meshID, uint32_t modelID);//TODO:为后面动态绑定新的mesh做准备

	std::unordered_map<uint32_t, std::vector<uint32_t>>* getModelBindMeshMap() { return &ModelBindMeshMap; }
	std::vector<uint32_t>& getModelBindMesh(uint32_t meshID) { return ModelBindMeshMap.at(meshID); }
	std::shared_ptr<modelInstance>& getModelInstanceByID(uint32_t modelID) { return bindModels[modelID]; }
	std::unordered_map<uint32_t, std::shared_ptr<modelInstance>>& getModelInstances() { return bindModels; }
	std::unordered_map<std::string, uint32_t>& getModelInstanceNameMap() { return ModelInstanceNameMap; }
	std::unordered_map<uint32_t, std::shared_ptr<Mesh>>* getMeshs() { return &bindMesh; }
	std::shared_ptr<Mesh> getMesh(uint32_t meshId) { return bindMesh[meshId]; }

	void setOffst(uint32_t offset) { Basic_DynamicUniformBufferOffset = offset; }
	uint32_t getOffset() const { return Basic_DynamicUniformBufferOffset; }


private:
	uint32_t generateModelId();

	void bindModelMeshMap(uint32_t modelID, uint32_t meshId);
};
