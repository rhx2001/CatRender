#pragma once
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <string>
class modelInstance
{
public:
	modelInstance(glm::vec3 position, std::string name, uint32_t uniformOffset);
	~modelInstance();
	void updateUniforms();

public:
	glm::mat4 transM;//变化矩阵
	std::string name;
	uint32_t uniformOffset;  // 在Uniform Buffer中的偏移量

private:

	

};