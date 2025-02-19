#include "App/CatRender.h"
#include "Gui/imgui.h"

CatRender::CatRender()
{
    GlfwWindow = std::make_unique<Window>();
    Render = std::make_unique<VulkanCore>();
    m_GUIManager = std::make_unique<GUIManager>();
}
CatRender::~CatRender() 
{

}
void CatRender::init()
{
    GlfwWindow->initWindow("CatRender", WIDTH, HEIGHT);
    Render->initVulkan(GlfwWindow->getWindowHandle(), m_GUIManager.get());
    m_GUIManager->init(GlfwWindow->getWindowHandle(), Render->getInstance(), Render.get());
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

        // GUI 新帧
        m_GUIManager->BeginFrame();

        ImGui::Begin("Control Panel");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        if (ImGui::Button("Rebuild Pipeline")) {
            // 处理按钮点击
        }
        ImGui::End();

        // Vulkan 绘制
        Render->drawFrame();
    }
    //vkDeviceWaitIdle(Render->getDecive());
}

void CatRender::cleanup()
{
    glfwTerminate();
}
