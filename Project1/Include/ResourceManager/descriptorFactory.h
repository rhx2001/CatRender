#pragma once
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>
class DescriptorFactory {
private:
    VkDevice device;
    std::vector<VkDescriptorPool> pools;
    std::unordered_map<size_t, VkDescriptorSetLayout> layoutCache; // 哈希值 -> 布局
    //TODO::layout重用

    // 创建新的描述符池
    VkDescriptorPool createPool(uint32_t maxSets, VkDescriptorPoolCreateFlags flags = 0);

    VkDescriptorPool getAvailablePool(uint32_t requiredSets, VkDescriptorSetLayout layout);

    // 从池中分配描述符集
    VkResult allocateDescriptorSets(VkDescriptorSetLayout layout, std::vector<VkDescriptorSet> set, uint32_t maxFrameInFlight);

    VkResult allocateDescriptorSet(VkDescriptorSetLayout layout, VkDescriptorSet& set);

public:
    DescriptorFactory(VkDevice device):device(device){}
    ~DescriptorFactory()=default;

    // 描述符布局构建器
    class LayoutBuilder {
    private:
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        DescriptorFactory* factory;

    public:
        LayoutBuilder(DescriptorFactory* factory):factory(factory){}

        // 添加各种类型的绑定
        LayoutBuilder& addBinding(uint32_t binding, VkDescriptorType type,
            VkShaderStageFlags stageFlags, uint32_t count = 1);
        LayoutBuilder& addUniformBuffer(uint32_t binding, VkShaderStageFlags stageFlags);
        LayoutBuilder& addDynamicUniformBuffer(uint32_t binding, VkShaderStageFlags stageFlags);
        LayoutBuilder& addStorageBuffer(uint32_t binding, VkShaderStageFlags stageFlags);
        LayoutBuilder& addCombinedImageSampler(uint32_t binding, VkShaderStageFlags stageFlags);

        // ... 其他类型

        // 创建布局
        VkDescriptorSetLayout build() const;
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
		SetBuilder(DescriptorFactory* factory, VkDescriptorSetLayout layout) :factory(factory) {
			factory->allocateDescriptorSet(layout, set);
		}

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
		SetBuilder& bindDynamicUniformBuffer(uint32_t binding, VkBuffer buffer,
			VkDeviceSize offset = 0, VkDeviceSize range = VK_WHOLE_SIZE);

        // 更新并返回描述符集
        VkDescriptorSet build();

        // 更新现有描述符集
        void update();
    };
    class FrameAwareSetBuilder// 专为多帧设计
    {
    private:
        std::vector<VkDescriptorSet> sets;
        std::vector<std::vector<VkWriteDescriptorSet>> writesPerFrame;
        DescriptorFactory* factory;

    public:
        FrameAwareSetBuilder(
            DescriptorFactory* factory,
            VkDescriptorSetLayout layout,
            uint32_t frameCount
        ) : factory(factory) {
            factory->allocateDescriptorSets(layout, sets, frameCount);
            writesPerFrame.resize(frameCount);
        }

        // 为特定帧添加绑定
        FrameAwareSetBuilder& bindBuffer(uint32_t frameIndex, uint32_t binding, VkBuffer buffer,
            VkDeviceSize offset, VkDeviceSize range, VkDescriptorType type
        );
        FrameAwareSetBuilder& bindImage(uint32_t frameIndex, uint32_t binding,VkImageView imageView,
            VkSampler sampler,VkDescriptorType type,VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );
        FrameAwareSetBuilder& bindUniformBuffer(uint32_t frameIndex, uint32_t binding, VkBuffer buffer,
            VkDeviceSize offset = 0, VkDeviceSize range = VK_WHOLE_SIZE);
		FrameAwareSetBuilder& bindDynamicUniformBuffer(uint32_t frameIndex, uint32_t binding, VkBuffer buffer,
			VkDeviceSize offset = 0, VkDeviceSize range = VK_WHOLE_SIZE);


        std::vector<VkDescriptorSet> build();

        void update();
    };


    // 创建布局构建器
    LayoutBuilder createLayout();

    // 创建集合构建器
    SetBuilder createSet(VkDescriptorSetLayout layout);

    FrameAwareSetBuilder createFrameAwareSet(VkDescriptorSetLayout layout, uint32_t frameCount);
    // 资源清理
    void cleanup();
};
