#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <ostream>
#include <glm/glm.hpp>
#include <string>
#include <glm/gtx/string_cast.hpp>
#include <ResourceManager/Mesh.h>

class modelInstance
{
public:
	modelInstance()
	{
		this->position = glm::vec3(0.0f);
		this->rotation = glm::vec3(0.0f);
		this->scale = glm::vec3(1.0f);
		this->transM = glm::mat4(1.0f);//变化矩阵
	}
	modelInstance(glm::mat4 model, std::string name, uint32_t uniformOffset)
		: transM(model),position(glm::vec3(0.0f)),rotation(glm::vec3(0.0f)),scale(glm::vec3(1.0f)), name(name), uniformOffset(uniformOffset)
	{
		std::cout << "modelInstance created: " << name << std::endl;
		std::cout << "position: " << glm::to_string(position) << std::endl;
		std::cout << "rotation: " << glm::to_string(rotation) << std::endl;
		std::cout << "scale: " << glm::to_string(scale) << std::endl;
	}

	struct dynamic_UniformBufferObject {
		alignas(16) glm::mat4 model;
	};

	//~modelInstance();
	void updateUniforms(void* mappedUniformBuffer);
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
	alignas(16)glm::mat4 transM;//变化矩阵
private:
	alignas(16)glm::vec3 position;
	alignas(16)glm::vec3 rotation;
	alignas(16)glm::vec3 scale;
public:
	std::string name;
private:
	uint32_t meshBindID;
	uint32_t materialBindID;
	uint32_t uniformOffset;  // 在Uniform Buffer中的偏移量


};