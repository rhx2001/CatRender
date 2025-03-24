#pragma once
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>
class DescriptorFactory {
private:
    VkDevice device;
    std::vector<VkDescriptorPool> pools;
    std::unordered_map<size_t, VkDescriptorSetLayout> layoutCache; // 哈希值 -> 布局

    // 创建新的描述符池
    VkDescriptorPool createPool(uint32_t maxSets, VkDescriptorPoolCreateFlags flags = 0);

    // 从池中分配描述符集
    VkResult allocateDescriptorSet(VkDescriptorSetLayout layout, VkDescriptorSet& set);

public:
    DescriptorFactory(VkDevice device);
    ~DescriptorFactory();

    // 描述符布局构建器
    class LayoutBuilder {
    private:
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        DescriptorFactory* factory;

    public:
        LayoutBuilder(DescriptorFactory* factory);

        // 添加各种类型的绑定
        LayoutBuilder& addBinding(uint32_t binding, VkDescriptorType type,
            VkShaderStageFlags stageFlags, uint32_t count = 1);
        LayoutBuilder& addUniformBuffer(uint32_t binding, VkShaderStageFlags stageFlags);
        LayoutBuilder& addStorageBuffer(uint32_t binding, VkShaderStageFlags stageFlags);
        LayoutBuilder& addCombinedImageSampler(uint32_t binding, VkShaderStageFlags stageFlags);
        // ... 其他类型

        // 创建布局
        VkDescriptorSetLayout build();
    };

    // 描述符集构建器
    class SetBuilder {
    private:
        VkDescriptorSet set;
        std::vector<VkWriteDescriptorSet> writes;
        std::vector<VkDescriptorBufferInfo> bufferInfos;
        std::vector<VkDescriptorImageInfo> imageInfos;
        DescriptorFactory* factory;

    public:
        SetBuilder(DescriptorFactory* factory, VkDescriptorSetLayout layout);

        // 绑定各种资源
        SetBuilder& bindBuffer(uint32_t binding, VkBuffer buffer,
            VkDeviceSize offset, VkDeviceSize range,
            VkDescriptorType type);
        SetBuilder& bindImage(uint32_t binding, VkImageView imageView,
            VkSampler sampler, VkDescriptorType type,
            VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        // ... 其他绑定

        // 绑定快捷方法
        SetBuilder& bindUniformBuffer(uint32_t binding, VkBuffer buffer,
            VkDeviceSize offset = 0, VkDeviceSize range = VK_WHOLE_SIZE);

        // 更新并返回描述符集
        VkDescriptorSet build();

        // 更新现有描述符集
        void update();
    };

    // 创建布局构建器
    LayoutBuilder createLayout();

    // 创建集合构建器
    SetBuilder createSet(VkDescriptorSetLayout layout);

    // 资源清理
    void cleanup();
};
