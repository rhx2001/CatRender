#include <ResourceManager/ModelInstance.h>

void modelInstance::updateUniforms(void* mappedUniformBuffer)
{
	dynamic_UniformBufferObject ubo{};

	glm::mat4 TransM = glm::mat4(1.0f); // 初始化为单位矩阵
	TransM = glm::translate(TransM, position);
	//std::cout << this->getPosition().x << " " << this->getPosition().y << " " << this->getPosition().z << std::endl;
	TransM = glm::rotate(TransM, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	TransM = glm::rotate(TransM, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	TransM = glm::rotate(TransM, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	TransM = glm::scale(TransM, scale);

	transM = TransM;
	ubo.model = TransM;
	char* data = static_cast<char*>(mappedUniformBuffer) + uniformOffset;
	memcpy(data, &ubo, sizeof(ubo));
}
