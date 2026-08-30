#include <iostream>
#include <cstring>
#include <ResourceManager/PBRMaterial.h>


PBRMaterial::PBRMaterial(uint32_t materialId, uint32_t offSet, const std::vector<std::shared_ptr<MaterialViewer>>& Viewers, std::string name)
:materialId(materialId), offSet(offSet), materialViewer(Viewers), name(name)
{
	pipelineConfig = {
		.vertexShader = VK_NULL_HANDLE,
		.fragmentShader = VK_NULL_HANDLE,
		.geometryShader = VK_NULL_HANDLE,
		.tessControlShader = VK_NULL_HANDLE,
		.tessEvalShader = VK_NULL_HANDLE,
		.vertexInput = {},
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = false,
		.dynamicViewportState = true,
		.viewports = {},
		.scissors = {},
		.rasterization = {
			VK_POLYGON_MODE_FILL,
			VK_CULL_MODE_BACK_BIT,
			VK_FRONT_FACE_CLOCKWISE,
			1.0f,
			false,
			false,
			false
		},

	};
}

void PBRMaterial::setMetallic(float value)
{
	params.metallicFactor = value;
}

void PBRMaterial::setRoughness(float value)
{
	params.roughnessFactor = value;
}

void PBRMaterial::setPipelineID(PipelineManager pipelineManager)
{

}

void PBRMaterial::UpdateUniformDate (void* mappedUniformBuffer) const
{
	MaterialBlock ubo{};
	ubo = params;
	
	char* data = static_cast<char*>(mappedUniformBuffer) + offSet;

	memcpy(data, &ubo, sizeof(ubo));
}


//这是我的basematerial以及我的pbrmaterial。bind部分我还没有写，因为我的是把cmd这部分使用materialmanager来管理
////绑定material对应的descriptorSet,这部分对应的是texture
//vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineParams.layout, 1, 1,
//	&materialManager->getMaterialViewer(MaterialID)->getDescriptorSet(static_cast<uint32_t>(currentFrame)), 0, nullptr);
////
//std::array<uint32_t, 1> MaterialOffset = { materialManager->getMaterial(MaterialID)->getOffSet() };
//vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineParams.layout, 2, 1,
//	&TextureUBODescriptorSets[currentFrame], 1, MaterialOffset.data());
//其中的materialManager->getMaterialViewer(MaterialID)->getDescriptorSet(static_cast<uint32_t>(currentFrame)我会写成循环来适应pbr的贴图。此外，我目前的渲染路径是这样的：randerpass->pipeline->material（这里material里面绑定了和material相关的mesh）->mesh->model(这里是和mesh绑定的mesh，保证一个mesh就渲染所有和mesh相关的model)。附件是我的basematerial以及我的pbrmaterial