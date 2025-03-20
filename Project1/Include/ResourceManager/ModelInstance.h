#pragma once
#include <glm/glm.hpp>
#include <string>
#include <ResourceManager/Mesh.h>

class modelInstance
{
public:
	modelInstance()
	{
		position = glm::vec3(0.0f);
		rotation = glm::vec3(0.0f);
		scale = glm::vec3(1.0f);
		transM = glm::mat4(1.0f);//变化矩阵
	}
	modelInstance(glm::mat4 model, std::string name, uint32_t uniformOffset)
		: transM(model), name(name), uniformOffset(uniformOffset)
	{
		position = glm::vec3(0.0f);
		rotation = glm::vec3(0.0f);
		scale = glm::vec3(1.0f);
	}
	//~modelInstance();
	void updateUniforms();
	//TODO：将unifrom的更新放到这里

	//allSet
	void setMesh(uint32_t MeshID) { meshBindID = MeshID; }
	void setMaterialID(uint32_t materialID) { materialBindID = materialID; }
	void setPosition(glm::vec3 pos) { position = pos; }
	void setRotation(glm::vec3 rot) { rotation = rot; }
	void setScale(glm::vec3 sca) { scale = sca; }

	//allGet
	glm::mat4 getTransM() const{ return transM; }
	glm::vec3 getPosition() const { return position; }
	glm::vec3 getRotation() const { return rotation; }
	glm::vec3 getScale() const { return scale; }


	uint32_t getOffset() const { return uniformOffset; }
public:
	glm::mat4 transM = glm::mat4(1.0f);//变化矩阵
	std::string name;

private:
	uint32_t meshBindID = 0;
	uint32_t materialBindID = 0;
	uint32_t uniformOffset;  // 在Uniform Buffer中的偏移量
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
};