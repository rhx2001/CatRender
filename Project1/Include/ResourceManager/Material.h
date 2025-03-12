#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
class Material {
public:
    uint32_t shaderId;          // 使用哪个着色器
    std::vector<uint32_t> bufferIds;  // 材质参数缓冲区
    std::vector<uint32_t> textureIds; // 材质纹理


    // 材质参数(可以设计不同的结构来存储不同种类的材质数据)
    struct {
        glm::vec4 baseColor;
        float metallic;
        float roughness;
        // 其他参数...
    } params;
};
