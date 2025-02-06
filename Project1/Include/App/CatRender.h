#pragma once

#include <memory>
#include "Render/VulkanCore.h"

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
	std::unique_ptr<Window> Window;
	std::unique_ptr<Scene> scene;

};

CatRender::CatRender()
{
}

CatRender::~CatRender()
{
}

