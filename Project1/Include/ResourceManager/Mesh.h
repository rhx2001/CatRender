#pragma once
#include <string>
#include <vulkan/vulkan_core.h>


class Mesh
{
private:
	VkBuffer VertexBuffer;
	VkBuffer indexBuffer;
	VkDeviceMemory memory;

public:
	Mesh();
	~Mesh();
	void loadMesh(const std::string& path);
public:
	std::string name;


};
