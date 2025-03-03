#include "ModelManager.h"

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
    uint32_t id = generateId();
    std::string name = "UnNamed" + std::to_string(id);
    bindModels[name] = std::make_shared<modelInstance>(glm::mat4(0), name, Basic_DynamicUniformBufferOffset * id);
}

void ModelManager::createModelInstance(std::string name)
{
    uint32_t id = generateId();
    bindModels[name] = std::make_shared<modelInstance>(glm::mat4(0), name, Basic_DynamicUniformBufferOffset * id);
}

void ModelManager::createModelInstance(std::string name, glm::mat4 transM)
{
    uint32_t id = generateId();
    bindModels[name] = std::make_shared<modelInstance>(transM, name, Basic_DynamicUniformBufferOffset * id);
}

void ModelManager::createModelInstance(std::string name, glm::mat4 transM, uint32_t meshId)
{
    uint32_t id = generateId();
    bindModels[name] = std::make_shared<modelInstance>(transM, name, Basic_DynamicUniformBufferOffset * id);
    bindModels[name]->setMesh(meshId);
}

uint32_t ModelManager::generateId()
{
    return ++modelId;
}


