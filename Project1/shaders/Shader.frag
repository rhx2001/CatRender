#version 450

// Set 1: ���������
layout(set = 1, binding = 0) uniform sampler2D texSampler;

// Set 2: ���ʲ�������C++�ṹ���ϸ�ƥ�䣩
layout(set = 2, binding = 0) uniform MaterialBlock {
    layout(offset = 0)  vec4 baseColorFactor;  // alignas(16)
    layout(offset = 16) float metallicFactor;  // alignas(4)
    layout(offset = 20) float roughnessFactor;
    layout(offset = 24) float occlusionStrength;
    // ����Ҫ���� reserved[32]
} material;

// ����
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

// ���
layout(location = 0) out vec4 outColor;

void main() {
    vec4 texColor = texture(texSampler, fragTexCoord);
    
    // ������ɫ��ϣ���������alphaͨ����
    outColor = vec4(material.baseColorFactor.rgb * texColor.rgb, texColor.a);
    
    // ������ԣ��ɵ����鿴���ʲ���
     //outColor = vec4(material.metallicFactor, material.roughnessFactor, 0.0, 1.0);
}