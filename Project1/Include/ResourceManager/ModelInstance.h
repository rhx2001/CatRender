#pragma once
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <ResourceManager/Mesh.h>

class modelInstance
{
public:
	modelInstance();
	modelInstance(glm::mat4 model, std::string name, uint32_t uniformOffset)
		: transM(model), name(name), uniformOffset(uniformOffset) {};
	~modelInstance();
	void updateUniforms();

public:
	glm::mat4 transM;//�仯����
	std::string name;
	uint32_t uniformOffset;  // ��Uniform Buffer�е�ƫ����

private:
	uint32_t meshBindID;
	

};