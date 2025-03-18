#include "ResourceManager/ModelManager.h"

void ModelManager::LoadMeshs(const std::vector<std::string>& paths)
{
	uint32_t id = 0;
    for (const auto& path : paths)
    {
        // ���� Mesh ���󲢳�ʼ��
        bindMesh[id] = std::make_shared<Mesh>(path, "Mesh" + std::to_string(id));

        // ���� LoadMesh ����
        bindMesh[id]->loadMesh(path);

        // ���� ID
        id++;
    }

}

void ModelManager::LoadMaterials()
{

}

void ModelManager::createModelInstance()
{
    uint32_t id = generateModelId();
    std::string name = "UnNamed" + std::to_string(id);
    bindModels[id] = std::make_shared<modelInstance>(glm::mat4(0), name, Basic_DynamicUniformBufferOffset * id);
}

void ModelManager::createModelInstance(std::string name)
{
    uint32_t id = generateModelId();
    bindModels[id] = std::make_shared<modelInstance>(glm::mat4(0), name, Basic_DynamicUniformBufferOffset * id);
}

void ModelManager::createModelInstance(uint32_t meshId)
{
    uint32_t id = generateModelId();
    std::string name = "UnNamed" + std::to_string(id);
    bindModels[id] = std::make_shared<modelInstance>(glm::mat4(0), name, Basic_DynamicUniformBufferOffset * id);
    bindModels[id]->setMesh(meshId);
    bindModelMeshMap(id, meshId);
}

void ModelManager::createModelInstance(std::string name, uint32_t meshId)
{
    uint32_t id = generateModelId();
    bindModels[id] = std::make_shared<modelInstance>(glm::mat4(0), name, Basic_DynamicUniformBufferOffset * id);
    bindModels[id]->setMesh(meshId);
    bindModelMeshMap(id, meshId);
}

void ModelManager::createModelInstance(std::string name, glm::mat4 transM)
{
    uint32_t id = generateModelId();
    bindModels[id] = std::make_shared<modelInstance>(transM, name, Basic_DynamicUniformBufferOffset * id);
}

void ModelManager::createModelInstance(glm::mat4 transM, uint32_t meshId)
{
    uint32_t id = generateModelId();
    std::string name = "UnNamed" + std::to_string(id);
    bindModels[id] = std::make_shared<modelInstance>(transM, name, Basic_DynamicUniformBufferOffset * id);
    bindModels[id]->setMesh(meshId);
    bindModelMeshMap(id, meshId);
}

void ModelManager::createModelInstance(std::string name, glm::mat4 transM, uint32_t meshId)
{
    uint32_t id = generateModelId();
    bindModels[id] = std::make_shared<modelInstance>(transM, name, Basic_DynamicUniformBufferOffset * id);
    bindModels[id]->setMesh(meshId);
    bindModelMeshMap(id, meshId);
}

void ModelManager::createModelInstance(std::string name, glm::mat4 transM, uint32_t meshId, uint32_t materialId)
{
    uint32_t id = generateModelId();
    bindModels[id] = std::make_shared<modelInstance>(transM, name, Basic_DynamicUniformBufferOffset * id);
    bindModels[id]->setMesh(meshId);
    bindModelMeshMap(id, meshId);
}

uint32_t ModelManager::generateModelId()
{
    return ++modelID_;
}

void ModelManager::bindModelMeshMap(uint32_t modelID, uint32_t meshId)
{
    if (ModelBindMeshMap.find(meshId) == ModelBindMeshMap.end())
    {
        ModelBindMeshMap[meshId] = std::vector<uint32_t>{ modelID };
    }

    // 如果没有找到，创建一个新的unordered_map并添加
    if (!found) {
        std::unordered_map<uint32_t, std::vector<uint32_t>> newMaterialMap;
        newMaterialMap[materialID] = { modelID };
        materialMaps.push_back(newMaterialMap);
    }
}


