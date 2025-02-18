#include "App/CatRender.h"
CatRender::CatRender()
{
    GlfwWindow = std::make_unique<Window>();
    Render = std::make_unique<VulkanCore>();
}
CatRender::~CatRender() 
{

}
void CatRender::init()
{

    GlfwWindow->initWindow("CatRender", WIDTH, HEIGHT);
	Render->initVulkan(GlfwWindow->getWindowHandle());

}

void CatRender::run()
{
    mainloop();
}

void CatRender::mainloop()
{
    GLFWwindow* window = GlfwWindow->getWindowHandle();
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        Render->drawFrame();
    }
    //vkDeviceWaitIdle(Render->getDecive());
}

void CatRender::cleanup()
{
    glfwTerminate();
}
