#pragma once
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>
class DescriptorFactory {
private:
    VkDevice device;
    std::vector<VkDescriptorPool> pools;
    std::unordered_map<size_t, VkDescriptorSetLayout> layoutCache; // ��ϣֵ -> ����

    // �����µ���������
    VkDescriptorPool createPool(uint32_t maxSets, VkDescriptorPoolCreateFlags flags = 0);

    // �ӳ��з�����������
    VkResult allocateDescriptorSet(VkDescriptorSetLayout layout, VkDescriptorSet& set);

public:
    DescriptorFactory(VkDevice device);
    ~DescriptorFactory();

    // ���������ֹ�����
    class LayoutBuilder {
    private:
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        DescriptorFactory* factory;

    public:
        LayoutBuilder(DescriptorFactory* factory);

        // ��Ӹ������͵İ�
        LayoutBuilder& addBinding(uint32_t binding, VkDescriptorType type,
            VkShaderStageFlags stageFlags, uint32_t count = 1);
        LayoutBuilder& addUniformBuffer(uint32_t binding, VkShaderStageFlags stageFlags);
        LayoutBuilder& addStorageBuffer(uint32_t binding, VkShaderStageFlags stageFlags);
        LayoutBuilder& addCombinedImageSampler(uint32_t binding, VkShaderStageFlags stageFlags);
        // ... ��������

        // ��������
        VkDescriptorSetLayout build();
    };

    // ��������������
    class SetBuilder {
    private:
        VkDescriptorSet set;
        std::vector<VkWriteDescriptorSet> writes;
        std::vector<VkDescriptorBufferInfo> bufferInfos;
        std::vector<VkDescriptorImageInfo> imageInfos;
        DescriptorFactory* factory;

    public:
        SetBuilder(DescriptorFactory* factory, VkDescriptorSetLayout layout);

        // �󶨸�����Դ
        SetBuilder& bindBuffer(uint32_t binding, VkBuffer buffer,
            VkDeviceSize offset, VkDeviceSize range,
            VkDescriptorType type);
        SetBuilder& bindImage(uint32_t binding, VkImageView imageView,
            VkSampler sampler, VkDescriptorType type,
            VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        // ... ������

        // �󶨿�ݷ���
        SetBuilder& bindUniformBuffer(uint32_t binding, VkBuffer buffer,
            VkDeviceSize offset = 0, VkDeviceSize range = VK_WHOLE_SIZE);

        // ���²�������������
        VkDescriptorSet build();

        // ����������������
        void update();
    };

    // �������ֹ�����
    LayoutBuilder createLayout();

    // �������Ϲ�����
    SetBuilder createSet(VkDescriptorSetLayout layout);

    // ��Դ����
    void cleanup();
};
