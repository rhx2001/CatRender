#include <ResourceManager/ModelInstance.h>

modelInstance::modelInstance(glm::vec3 position, std::string name, VulkanCore::UniformBufferObject* mappedUniformData)
    : transM(glm::translate(glm::mat4(1.0f), position)), name(name), mappedUniformData(mappedUniformData)
{
    // 构造函数体可以为空
}