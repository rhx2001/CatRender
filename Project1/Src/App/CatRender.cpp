#include "App/CatRender.h"
CatRender::CatRender()
{
    GlfwWindow = std::make_unique<Window>();
    Renderer = std::make_unique<VulkanCore>();
}
CatRender::~CatRender() 
{

}
void CatRender::init()
{

    GlfwWindow->initWindow("CatRender", WIDTH, HEIGHT);
	Renderer->initVulkan();

}

void CatRender::mainloop()
{
    GLFWwindow* window = GlfwWindow->getWindowHandle();
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        //drawFrame();
    }
    //vkDeviceWaitIdle(device);
}
