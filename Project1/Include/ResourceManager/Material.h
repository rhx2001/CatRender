#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
class Material {
public:
    uint32_t shaderId;          // ʹ���ĸ���ɫ��
    std::vector<uint32_t> bufferIds;  // ���ʲ���������
    std::vector<uint32_t> textureIds; // ��������


    // ���ʲ���(������Ʋ�ͬ�Ľṹ���洢��ͬ����Ĳ�������)
    struct {
        glm::vec4 baseColor;
        float metallic;
        float roughness;
        // ��������...
    } params;
};
