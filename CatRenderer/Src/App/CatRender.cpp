#include "App/CatRender.h"
#include "Gui/imgui.h"

CatRender::CatRender()
{
    GlfwWindow = std::make_unique<Window>();
    Renderer = std::make_unique<VulkanCore>();
    m_GUIManager = std::make_unique<GUIManager>();
}
CatRender::~CatRender() 
{

}
void CatRender::init()
{
    GlfwWindow->initWindow("CatRender", WIDTH, HEIGHT);
    Renderer->initVulkan(GlfwWindow->getWindowHandle(), m_GUIManager.get());
    m_GUIManager->init(GlfwWindow->getWindowHandle(), Renderer->getInstance(), Renderer.get());
}

void CatRender::run()
{
    mainloop();
}

void CatRender::mainloop()
{
    GLFWwindow* window = GlfwWindow->getWindowHandle();
    while (!glfwWindowShouldClose(window)) 
    {
        glfwPollEvents();

        // Vulkan »æÖÆ
        Renderer->drawFrame();
    }
    //vkDeviceWaitIdle(Renderer->getDecive());
}

void CatRender::cleanup()
{
    glfwTerminate();
}
