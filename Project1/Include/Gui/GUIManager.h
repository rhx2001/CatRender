#pragma once
#include "imgui.h"
#include "Render/VulkanCore.h"

class GUIManager
{
public:
	GUIManager(GLFWwindow* window, VkInstance instance, VulkanCore* vulkanCore);
	~GUIManager();

public:
	void init();					// 初始化GUI
	void BeginFrame();               // 开始新帧
	void EndFrame(VkCommandBuffer cmd); // 结束帧并提交绘制命令
	void OnResize();                 // 窗口大小变化处理
	void render();				   // 渲染GUI
	void cleanup();				   // 清理GUI

private:
	GLFWwindow* m_Window;
	VkInstance m_Instance;
	VulkanCore* m_VulkanCore;
	VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

	void CreateDescriptorPool();     // 创建 ImGui 专用描述符池
};