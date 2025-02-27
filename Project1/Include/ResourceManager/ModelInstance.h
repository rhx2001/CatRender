#pragma once
#include <glm/glm.hpp>
#include <string>
class modelInstance
{
public:
	modelInstance();
	modelInstance(glm::mat4 model, std::string name, uint32_t uniformOffset)
		: transM(model), name(name), uniformOffset(uniformOffset) {};
	~modelInstance();
	void updateUniforms();

public:
	glm::mat4 transM;//变化矩阵
	std::string name;
	uint32_t uniformOffset;  // 在Uniform Buffer中的偏移量

private:

	

};