#pragma once
#include <queue>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>
class DescriptorFactory {
private:
    VkDevice& device;
    std::vector<VkDescriptorPool> pools;
    std::priority_queue < std::pair<uint32_t,uint32_t>, std::vector<std::pair<uint32_t, uint32_t>>, std::less<std::pair<uint32_t, uint32_t>>> poolSizes;
    std::unordered_map<size_t, VkDescriptorSetLayout> layoutCache; // ��ϣֵ -> ����
    //TODO::layout����

    // �����µ���������
    VkDescriptorPool createPool(uint32_t maxSets, VkDescriptorPoolCreateFlags flags = 0);

    VkDescriptorPool getAvailablePool(uint32_t requiredSets, VkDescriptorSetLayout layout);

    bool checkPoolHasSpace(VkDescriptorPool pool, uint32_t requiredSets, VkDescriptorSetLayout layout);

    // �ӳ��з�����������
    VkResult allocateDescriptorSets(VkDescriptorSetLayout layout, std::vector<VkDescriptorSet>& set, uint32_t maxFrameInFlight);

    VkResult allocateDescriptorSet(VkDescriptorSetLayout layout, VkDescriptorSet& set);

public:
    DescriptorFactory(VkDevice& device) :device(device) {}
    ~DescriptorFactory()=default;

    // ���������ֹ�����
    class LayoutBuilder {
    private:
        std::vector<VkDescriptorSetLayoutBinding> bindings;
        DescriptorFactory* factory;

    public:
        LayoutBuilder(DescriptorFactory* factory):factory(factory){}

        // ��Ӹ������͵İ�
        LayoutBuilder& addBinding(uint32_t binding, VkDescriptorType type,
            VkShaderStageFlags stageFlags, uint32_t count = 1);
        LayoutBuilder& addUniformBuffer(uint32_t binding, VkShaderStageFlags stageFlags);
        LayoutBuilder& addDynamicUniformBuffer(uint32_t binding, VkShaderStageFlags stageFlags);
        LayoutBuilder& addStorageBuffer(uint32_t binding, VkShaderStageFlags stageFlags);
        LayoutBuilder& addCombinedImageSampler(uint32_t binding, VkShaderStageFlags stageFlags);

        // ... ��������

        // ��������
        VkDescriptorSetLayout build() const;
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
		SetBuilder(DescriptorFactory* factory, VkDescriptorSetLayout layout) :factory(factory) {
			factory->allocateDescriptorSet(layout, set);
		}

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
		SetBuilder& bindDynamicUniformBuffer(uint32_t binding, VkBuffer buffer,
			VkDeviceSize offset = 0, VkDeviceSize range = VK_WHOLE_SIZE);

        // ���²�������������
        VkDescriptorSet build();

        // ����������������
        void update();
    };
    class FrameAwareSetBuilder// רΪ��֡���
    {
    private:
        std::vector<VkDescriptorSet> sets;
        std::vector<std::vector<VkWriteDescriptorSet>> writesPerFrame;
        DescriptorFactory* factory;
        std::vector<std::vector<VkDescriptorBufferInfo>> bufferInfosPerFrame;

    public:
        FrameAwareSetBuilder(
            DescriptorFactory* factory,
            VkDescriptorSetLayout layout,
            uint32_t frameCount
        ) : factory(factory),frameCount(frameCount) {
            factory->allocateDescriptorSets(layout, sets, frameCount);
            writesPerFrame.resize(frameCount);
            bufferInfosPerFrame.resize(frameCount);
        }
        uint32_t frameCount;

        // Ϊ�ض�֡��Ӱ�
        FrameAwareSetBuilder& bindBuffer(uint32_t binding, const std::vector<VkBuffer>& buffer,
            VkDeviceSize offset, VkDeviceSize range, VkDescriptorType type
        );
        FrameAwareSetBuilder& bindImage(uint32_t binding, VkImageView imageView,
            VkSampler sampler,VkDescriptorType type,VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );
        FrameAwareSetBuilder& bindUniformBuffer(uint32_t binding, const std::vector<VkBuffer>& buffer,
            VkDeviceSize offset = 0, VkDeviceSize range = VK_WHOLE_SIZE);
		FrameAwareSetBuilder& bindDynamicUniformBuffer(uint32_t binding, const std::vector<VkBuffer>& buffer,
			VkDeviceSize offset = 0, VkDeviceSize range = VK_WHOLE_SIZE);
        FrameAwareSetBuilder& bindCombinedImageSampler(uint32_t binding, VkImageView imageView,
            VkSampler sampler, VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


        std::vector<VkDescriptorSet> build();

        std::vector<VkDescriptorSet>  update();
    };


    // �������ֹ�����
    LayoutBuilder createLayout();

    // �������Ϲ�����
    SetBuilder createSet(VkDescriptorSetLayout layout);

    FrameAwareSetBuilder createFrameAwareSet(VkDescriptorSetLayout layout, uint32_t frameCount);
    // ��Դ����
    void cleanup();
};
