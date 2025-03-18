#pragma once
#include <glm/glm.hpp>
#include <string>
#include <ResourceManager/Mesh.h>

class modelInstance
{
public:
	modelInstance();
	modelInstance(glm::mat4 model, std::string name, uint32_t uniformOffset)
		: transM(model), name(name), uniformOffset(uniformOffset) {}
	//~modelInstance();
	void updateUniforms();
	//TODO：将unifrom的更新放到这里
	void setMesh(uint32_t MeshID) { meshBindID = MeshID; }
	void setMaterialID(uint32_t materialID) { materialBindID = materialID; }

public:
	glm::mat4 transM;//变化矩阵
	std::string name;
	uint32_t uniformOffset;  // 在Uniform Buffer中的偏移量

private:
	uint32_t meshBindID = 0;
	uint32_t materialBindID = 0;

};