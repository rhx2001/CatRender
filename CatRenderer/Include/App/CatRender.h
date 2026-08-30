#pragma once
#include "Renderer/VulkanCore.h"
#include <memory>
#include "Gui/GUIManager.h"

const int WIDTH = 640;
const int HEIGHT = 480;

class CatRender
{
public:
	CatRender();
	~CatRender();
	void init();
	void run();
	void mainloop();
	void cleanup();
	

private:
	std::unique_ptr<VulkanCore> Renderer;
	std::unique_ptr<Window> GlfwWindow;
	std::unique_ptr<GUIManager> m_GUIManager;
};


