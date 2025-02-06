#include "App/CatRender.h"
inline void CatRender::init()
{
	Window->initWindow("CatRender", WIDTH, HEIGHT);
	Renderer->initVulkan();

}
