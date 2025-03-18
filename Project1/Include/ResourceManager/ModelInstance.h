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
	//TODO����unifrom�ĸ��·ŵ�����
	void setMesh(uint32_t MeshID) { meshBindID = MeshID; }
	void setMaterialID(uint32_t materialID) { materialBindID = materialID; }

public:
	glm::mat4 transM;//�仯����
	std::string name;
	uint32_t uniformOffset;  // ��Uniform Buffer�е�ƫ����

private:
	uint32_t meshBindID = 0;
	uint32_t materialBindID = 0;

};