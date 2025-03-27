#include "ResourceManager/DescriptorFactory.h"

VkDescriptorPool DescriptorFactory::createPool(uint32_t maxSets, VkDescriptorPoolCreateFlags flags)
{
    VkDescriptorPool pool;
    std::vector<VkDescriptorPoolSize> pool_sizes = {
{ VK_DESCRIPTOR_TYPE_SAMPLER, 500 },
{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 500 },
{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 500 },
{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 500 },
{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 500 },
{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 500 },
{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 500 },
{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 500 },
{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 500 },
{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 500 },
{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 500 }
    };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = flags;
    pool_info.maxSets = maxSets * static_cast<uint32_t>(pool_sizes.size());
    pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    pool_info.pPoolSizes = pool_sizes.data();
    vkCreateDescriptorPool(device, &pool_info, nullptr, &pool);
	pools.push_back(pool);
	poolSizes.emplace(pool_info.maxSets, static_cast<uint32_t>(pools.size() - 1));
	return pool;
}

VkResult DescriptorFactory::allocateDescriptorSets(VkDescriptorSetLayout layout, std::vector<VkDescriptorSet>& set, uint32_t maxFrameInFlight)
{
    std::vector<VkDescriptorSetLayout> layouts(maxFrameInFlight, layout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = getAvailablePool(static_cast<uint32_t>(set.size()), layout);
    allocInfo.descriptorSetCount = static_cast<uint32_t>(maxFrameInFlight);
    allocInfo.pSetLayouts = layouts.data();
    set.resize(maxFrameInFlight);
    return vkAllocateDescriptorSets(device, &allocInfo, set.data());
}

VkResult DescriptorFactory::allocateDescriptorSet(VkDescriptorSetLayout layout, VkDescriptorSet& set)
{
    return VkResult();
}

DescriptorFactory::LayoutBuilder DescriptorFactory::createLayout()
{
    return { this };
}

DescriptorFactory::SetBuilder DescriptorFactory::createSet(VkDescriptorSetLayout layout)
{
    return { this, layout };
}

DescriptorFactory::FrameAwareSetBuilder DescriptorFactory::createFrameAwareSet(VkDescriptorSetLayout layout, uint32_t frameCount)
{
    return { this, layout, frameCount };
}

void DescriptorFactory::cleanup()
{

}


VkDescriptorPool DescriptorFactory::getAvailablePool(uint32_t requiredSets, VkDescriptorSetLayout layout) {
	if (pools.empty())
	{
		return createPool(500);
	}
    // 优先查找有足够空间的现有池
	auto [size, poolID] = poolSizes.top();
	poolSizes.pop();
	if (size > requiredSets)
	{
		size -= requiredSets;
		poolSizes.emplace(size, poolID);
		return pools.at(poolID);
	}
	else
	{
		return createPool(500);
	}

}
//TODO:在第一个大的pool用完的时候，创建一个新的适用于layout得pool
bool DescriptorFactory::checkPoolHasSpace(VkDescriptorPool pool, uint32_t requiredSets, VkDescriptorSetLayout layout)
{
	return false;
}

DescriptorFactory::FrameAwareSetBuilder& DescriptorFactory::FrameAwareSetBuilder::bindBuffer(uint32_t binding, const std::vector<VkBuffer>& buffer, VkDeviceSize offset, VkDeviceSize range, VkDescriptorType type)
{
	for (uint32_t frameIndex = 0; frameIndex < frameCount; frameIndex++) {
		//VkDescriptorBufferInfo bufferInfo{ buffer[frameIndex], offset, range};
		bufferInfosPerFrame[frameIndex].push_back({ buffer[frameIndex], offset, range });
		writesPerFrame[frameIndex].push_back(
			VkWriteDescriptorSet{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				sets[frameIndex],
				binding,
				0,
				1,
				type,
				nullptr,
				& bufferInfosPerFrame[frameIndex].back(),
				nullptr });
	}
	return *this;
}

DescriptorFactory::FrameAwareSetBuilder& DescriptorFactory::FrameAwareSetBuilder::bindImage(uint32_t binding, VkImageView imageView, VkSampler sampler, VkDescriptorType type, VkImageLayout ImageLayout)
{
	for (size_t frameIndex = 0; frameIndex < frameCount; frameIndex++) {
		VkDescriptorImageInfo imageInfo{ sampler, imageView, ImageLayout };
		writesPerFrame[frameIndex].push_back(
			VkWriteDescriptorSet{
				VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				nullptr,
				sets[frameIndex],
				binding,
				0,
				1,
				type,
				&imageInfo,
				nullptr,
				nullptr
			});
	}
	return *this;
}

DescriptorFactory::FrameAwareSetBuilder& DescriptorFactory::FrameAwareSetBuilder::bindUniformBuffer(uint32_t binding, const std::vector<VkBuffer>& buffer, VkDeviceSize offset, VkDeviceSize range)
{
	return bindBuffer(binding, buffer, offset, range, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
}

DescriptorFactory::FrameAwareSetBuilder& DescriptorFactory::FrameAwareSetBuilder::bindDynamicUniformBuffer(uint32_t binding, const std::vector<VkBuffer>& buffer, VkDeviceSize offset, VkDeviceSize range)
{
	return bindBuffer(binding, buffer, offset, range, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC);
}

DescriptorFactory::FrameAwareSetBuilder& DescriptorFactory::FrameAwareSetBuilder::bindCombinedImageSampler(uint32_t binding, VkImageView imageView, VkSampler sampler, VkImageLayout ImageLayout)
{

	return bindImage(binding, imageView, sampler, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, ImageLayout);
}



std::vector<VkDescriptorSet> DescriptorFactory::FrameAwareSetBuilder::build()
{
    for (size_t i = 0; i < writesPerFrame.size(); ++i) {
        if (!writesPerFrame[i].empty()) {
            vkUpdateDescriptorSets(
                factory->device,
                static_cast<uint32_t>(writesPerFrame[i].size()),
                writesPerFrame[i].data(),
                0,
                nullptr
            );
        }
    }
	return sets;
}

std::vector<VkDescriptorSet> DescriptorFactory::FrameAwareSetBuilder::update()
{
	build();
	return sets;
}

DescriptorFactory::LayoutBuilder& DescriptorFactory::LayoutBuilder::addBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t count)
{
	bindings.push_back({ binding, type, count, stageFlags, nullptr });
	return *this;
    // TODO: 在此处插入 return 语句
}

DescriptorFactory::LayoutBuilder& DescriptorFactory::LayoutBuilder::addUniformBuffer(uint32_t binding, VkShaderStageFlags stageFlags)
{
	return addBinding(binding, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, stageFlags);
}

DescriptorFactory::LayoutBuilder& DescriptorFactory::LayoutBuilder::addDynamicUniformBuffer(uint32_t binding, VkShaderStageFlags stageFlags)
{
	return addBinding(binding, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, stageFlags);
    // TODO: 在此处插入 return 语句
}

DescriptorFactory::LayoutBuilder& DescriptorFactory::LayoutBuilder::addStorageBuffer(uint32_t binding, VkShaderStageFlags stageFlags)
{
	return addBinding(binding, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, stageFlags);
}

DescriptorFactory::LayoutBuilder& DescriptorFactory::LayoutBuilder::addCombinedImageSampler(uint32_t binding, VkShaderStageFlags stageFlags)
{
	return addBinding(binding, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, stageFlags);
}

VkDescriptorSetLayout DescriptorFactory::LayoutBuilder::build() const
{
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();
	VkDescriptorSetLayout layout;
	vkCreateDescriptorSetLayout(factory->device, &layoutInfo, nullptr, &layout);
	return layout;
}

DescriptorFactory::SetBuilder& DescriptorFactory::SetBuilder::bindBuffer(uint32_t binding, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range, VkDescriptorType type)
{
	bufferInfos.push_back({ buffer, offset, range });
	writes.push_back(
		VkWriteDescriptorSet{
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			nullptr,
			set,
			binding,
			0,
			1,
			type,
			nullptr,
			&bufferInfos.back(),
			nullptr
		});
	return *this;
}

DescriptorFactory::SetBuilder& DescriptorFactory::SetBuilder::bindImage(uint32_t binding, VkImageView imageView, VkSampler sampler, VkDescriptorType type, VkImageLayout layout)
{
	imageInfos.push_back({ sampler, imageView, layout });
	writes.push_back(
		VkWriteDescriptorSet{
			VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			nullptr,
			set,
			binding,
			0,
			1,
			type,
			&imageInfos.back(),
			nullptr,
			nullptr
		});
	return *this;
}

DescriptorFactory::SetBuilder& DescriptorFactory::SetBuilder::bindUniformBuffer(uint32_t binding, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range)
{
	return bindBuffer(binding, buffer, offset, range, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
}

DescriptorFactory::SetBuilder& DescriptorFactory::SetBuilder::bindDynamicUniformBuffer(uint32_t binding, VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range)
{
	return bindBuffer(binding, buffer, offset, range, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC);
}

VkDescriptorSet DescriptorFactory::SetBuilder::build()
{
	vkUpdateDescriptorSets(
		factory->device,
		static_cast<uint32_t>(writes.size()),
		writes.data(),
		0,
		nullptr
	);
	return set;
}

void DescriptorFactory::SetBuilder::update()
{
	vkUpdateDescriptorSets(
		factory->device,
		static_cast<uint32_t>(writes.size()),
		writes.data(),
		0,
		nullptr
	);
}
