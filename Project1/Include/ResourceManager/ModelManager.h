#pragma once
#include <ResourceManager/ModelInstance.h>
class ModelManager
{
public:


private:
	std::unordered_map<uint32_t, Mesh> bindMesh;
	std::unordered_map<std::string, modelInstance> bindModel;

public:
	ModelManager();
	~ModelManager();
};
