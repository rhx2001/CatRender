#include "Gui/GUIManager.h"
#include "Gui/imgui.h"
#include "Gui/imgui_impl_glfw.h"
#include "Gui/imgui_impl_vulkan.h"
#include "Renderer/VulkanCore.h"

GUIManager::GUIManager(){}

GUIManager::~GUIManager()
{
    vkDeviceWaitIdle(m_VulkanCore->getDevice());
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    if (m_DescriptorPool != VK_NULL_HANDLE) {
		vkDestroyDescriptorPool(m_VulkanCore->getDevice(), m_DescriptorPool, nullptr);//��������imgui����������
    }
}

void GUIManager::init(GLFWwindow* window, VkInstance instance, VulkanCore* vulkanCore)
{
    m_Window = window;
    m_Instance = instance;
	m_VulkanCore = vulkanCore;
    // ��ʼ�� ImGui ������
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // ������ʽ
    ImGui::StyleColorsDark();

    // ��ʼ�� GLFW ���
    ImGui_ImplGlfw_InitForVulkan(m_Window, true);

    // ���� Vulkan ��Դ
    CreateDescriptorPool();

    // ��ʼ�� Vulkan ���
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
    init_info.Subpass = 1;

    ImGui_ImplVulkan_Init(&init_info);

    ImGui_ImplVulkan_CreateFontsTexture();

}

void GUIManager::BeginFrame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Control Panel");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    if (ImGui::Button("Rebuild Pipeline")) {
        // ����ť���
    }
    ImGui::End();

	ImGui::Begin("Camera");
    Camera* camera = m_VulkanCore->camera;
    static float fov = camera->getFov(); // ��ʼ��Ϊ��ǰ�����FOV
    // Near and Far Planes
    static float znear = camera->getNearClip(); // ��ʼ��Ϊ��ǰ����Ľ��ü���
    static float zfar = camera->getFarClip();   // ��ʼ��Ϊ��ǰ�����Զ�ü���

    if (ImGui::SliderFloat("FOV", &fov, 30.0f, 120.0f)|| ImGui::InputFloat("Near Plane", &znear)|| ImGui::InputFloat("Far Plane", &zfar)) {
        // ��ȡ��ǰ���ڵĿ�߱�
        float aspectRatio = m_VulkanCore->getAspectRatio(); // �������з�����ȡ���ڿ�߱�
        camera->setPerspective(fov, aspectRatio, znear, zfar);
    }

    static glm::vec3 pos = camera->position;
    if (ImGui::DragFloat3("Position", &pos.x)) {
        camera->setPosition(pos);
    }

    static glm::vec3 rotation = camera->rotation;
    if (ImGui::DragFloat3("Rotation",&rotation.x,1.0f,-180.0f,180.0f))
    {
        camera->setRotation(rotation);
    }
    ImGui::End();

    ImGui::Begin("Models");
	auto& modelInstances = m_VulkanCore->getModelManager().getModelInstances();
	if (ImGui::CollapsingHeader("Models")) {
		for (auto& [modelID, modelInstance] : modelInstances) {
			if (ImGui::TreeNode(modelInstance->name.c_str())) {
				ImGui::Text("Model ID: %d", modelID);
                glm::vec3 position = modelInstance->getPosition();
                if (ImGui::DragFloat3("Position", &position.x)) {
                    modelInstance->setPosition(position);
                }
                glm::vec3 rotation = modelInstance->getRotation();
                if (ImGui::DragFloat3("Rotation", &rotation.x)) {
                    modelInstance->setRotation(rotation);
                }
                glm::vec3 scale = modelInstance->getScale();
                if (ImGui::DragFloat3("Scale", &scale.x)) {
                    modelInstance->setScale(scale);
                }
				ImGui::TreePop();
			}
		}
	}
	ImGui::End();

    //ImGui::Begin("Materials");
    //auto& Materials = m_VulkanCore->getMaterialManager().getMaterials();
    //for (auto& [materialID, material] : Materials) {
    //    if (ImGui::TreeNode(material->name.c_str())) {
    //        ImGui::Text("Model ID: %d", materialID);
    //        glm::vec4 baseColor = material->get
    //        if (ImGui::DragFloat3("Position", &position.x)) {
    //            modelInstance->setPosition(position);
    //        }
    //        glm::vec3 rotation = modelInstance->getRotation();
    //        if (ImGui::DragFloat3("Rotation", &rotation.x)) {
    //            modelInstance->setRotation(rotation);
    //        }
    //        glm::vec3 scale = modelInstance->getScale();
    //        if (ImGui::DragFloat3("Scale", &scale.x)) {
    //            modelInstance->setScale(scale);
    //        }
    //        ImGui::TreePop();
    //    }
    //}

    //ImGui::End();


    // ʾ�������Ƶ��Դ���
    ImGui::ShowDemoWindow();
}

void GUIManager::EndFrame(VkCommandBuffer cmd) {
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
}

void GUIManager::OnResize() {
    // �ؽ��������������Ҫ��
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
    { VK_DESCRIPTOR_TYPE_SAMPLER, 500 },
    { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 500 },
    { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 500 },
    { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 500 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 500 },
    { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 500 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 500 },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 500 },
    { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 500 },
    { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 500 },
    { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 500 }
    };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 500 * IM_ARRAYSIZE(pool_sizes);
    pool_info.poolSizeCount = static_cast<uint32_t>(IM_ARRAYSIZE(pool_sizes));
    pool_info.pPoolSizes = pool_sizes;
    VK_CHECK(vkCreateDescriptorPool(m_VulkanCore->getDevice(), &pool_info, nullptr, &m_DescriptorPool))
}
