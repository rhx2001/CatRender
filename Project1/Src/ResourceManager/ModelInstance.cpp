#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <ResourceManager/ModelInstance.h>

modelInstance::modelInstance(glm::vec3 position, std::string name, uint32_t uniformOffset)
    : transM(glm::translate(glm::mat4(1.0f), position)), name(name), uniformOffset(uniformOffset)
{
    // 构造函数体可以为空
}