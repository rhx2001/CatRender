#include "Gui/GUIManager.h"
#include "Gui/imgui.h"
#include "Gui/imgui_impl_glfw.h"
#include "Gui/imgui_impl_vulkan.h"
#include "Renderer/VulkanCore.h"

GUIManager::GUIManager(){};

GUIManager::~GUIManager()
{
    vkDeviceWaitIdle(m_VulkanCore->getDevice());
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (m_DescriptorPool != VK_NULL_HANDLE) {
		vkDestroyDescriptorPool(m_VulkanCore->getDevice(), m_DescriptorPool, nullptr);//销毁属于imgui的描述符池
    }
}

void GUIManager::init(GLFWwindow* window, VkInstance instance, VulkanCore* vulkanCore)
{
    m_Window = window;
    m_Instance = instance;
	m_VulkanCore = vulkanCore;
    // 初始化 ImGui 上下文
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // 设置样式
    ImGui::StyleColorsDark();

    // 初始化 GLFW 后端
    ImGui_ImplGlfw_InitForVulkan(m_Window, true);

    // 创建 Vulkan 资源
    CreateDescriptorPool();

    // 初始化 Vulkan 后端
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = m_Instance;
    init_info.PhysicalDevice = m_VulkanCore->getPhysicalDevice();
    init_info.Device = m_VulkanCore->getDevice();
    init_info.QueueFamily = m_VulkanCore->findQueueFamilies(m_VulkanCore->getPhysicalDevice()).graphicsFamily.value();
    init_info.Queue = m_VulkanCore->getGraphicsQueue();
    init_info.DescriptorPool = m_DescriptorPool;
    init_info.MinImageCount = m_VulkanCore->getSwapChainImageCount();
    init_info.ImageCount = m_VulkanCore->getSwapChainImageCount();
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.RenderPass = m_VulkanCore->getRenderPass();

    ImGui_ImplVulkan_Init(&init_info);

    ImGui_ImplVulkan_CreateFontsTexture();

}

void GUIManager::BeginFrame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();



    // 示例：绘制调试窗口
    ImGui::ShowDemoWindow();
}

void GUIManager::EndFrame(VkCommandBuffer cmd) {
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
}

void GUIManager::OnResize() {
    // 重建字体纹理（如果需要）
    ImGui_ImplVulkan_SetMinImageCount(m_VulkanCore->getSwapChainImageCount());
}

void GUIManager::render()
{

}

void GUIManager::cleanup()
{

}

void GUIManager::CreateDescriptorPool()
{
    VkDescriptorPoolSize pool_sizes[] = {
    { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
    { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
    { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
    { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
    { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
    pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;
    VK_CHECK(vkCreateDescriptorPool(m_VulkanCore->getDevice(), &pool_info, nullptr, &m_DescriptorPool));
}
