#include "MaterialViewer.h"

void MaterialViewer::setDescriptorSet(uint32_t FRAME_IN_FLIGHT, VkDescriptorSet descriptorSet)
{
	descriptorSets[FRAME_IN_FLIGHT] = descriptorSet;
}
