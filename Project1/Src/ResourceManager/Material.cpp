#include <iostream>
#include <ResourceManager/Material.h>


void Material::setMetallic(float value)
{
	params.metallicFactor = value;
}

void Material::setRoughness(float value)
{
	params.roughnessFactor = value;
}

void Material::updateUniformData (void* mappedUniformBuffer) const 
{
	MaterialBlock ubo{};
	ubo = params;
	
	char* data = static_cast<char*>(mappedUniformBuffer) + offSet;

	memcpy(data, &ubo, sizeof(ubo));
}
