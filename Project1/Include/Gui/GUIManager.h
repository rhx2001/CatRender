#pragma once
#include "imgui.h"
#include "Render/VulkanCore.h"

class GUIManager
{
public:
	GUIManager(GLFWwindow* window, VkInstance instance, VulkanCore* vulkanCore);
	~GUIManager();

public:
	void init();					// ��ʼ��GUI
	void BeginFrame();               // ��ʼ��֡
	void EndFrame(VkCommandBuffer cmd); // ����֡���ύ��������
	void OnResize();                 // ���ڴ�С�仯����
	void render();				   // ��ȾGUI
	void cleanup();				   // ����GUI

private:
	GLFWwindow* m_Window;
	VkInstance m_Instance;
	VulkanCore* m_VulkanCore;
	VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

	void CreateDescriptorPool();     // ���� ImGui ר����������
};