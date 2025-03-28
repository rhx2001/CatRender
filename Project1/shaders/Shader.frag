#version 450

// Set 1: 纹理采样器
layout(set = 1, binding = 0) uniform sampler2D texSampler;

// Set 2: 材质参数（与C++结构体严格匹配）
layout(set = 2, binding = 0) uniform MaterialBlock {
    layout(offset = 0)  vec4 baseColorFactor;  // alignas(16)
    layout(offset = 16) float metallicFactor;  // alignas(4)
    layout(offset = 20) float roughnessFactor;
    layout(offset = 24) float occlusionStrength;
    // 不需要声明 reserved[32]
} material;

// 输入
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

// 输出
layout(location = 0) out vec4 outColor;

void main() {
    vec4 texColor = texture(texSampler, fragTexCoord);
    
    // 基础颜色混合（保留纹理alpha通道）
    outColor = vec4(material.baseColorFactor.rgb * texColor.rgb, texColor.a);
    
    // 如需调试，可单独查看材质参数
     //outColor = vec4(material.metallicFactor, material.roughnessFactor, 0.0, 1.0);
}