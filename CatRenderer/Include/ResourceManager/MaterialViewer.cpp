#include "MaterialViewer.h"


void MaterialViewer::setDescriptorSetByFrame(uint32_t FRAME_IN_FLIGHT, VkDescriptorSet descriptorSet)
{
	m_descriptorSets[FRAME_IN_FLIGHT] = descriptorSet;
}
