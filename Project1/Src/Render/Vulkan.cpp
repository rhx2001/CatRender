#include"Render/VulkanCore.h"


inline void VulkanCore::initVulkan()
{
	createInstance();
}

inline void VulkanCore::createInstance()
{
	if (enableValidationLayers && !checkValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}


	uint32_t apiVersion;
	VkResult VersionResult =  vkEnumerateInstanceVersion(&apiVersion);
	if (VersionResult != VK_SUCCESS) {
		throw std::runtime_error("Failed to query the highest supported Vulkan version.");
	}//�����豸֧�ֵ����汾��

	std::cout << "Max Vulkan Version is" << apiVersion << "\n";

	VkApplicationInfo appInfo{};//����app��Ϣ�������汾�Ű汾���Ƶȵ�
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "CatRender";
	appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0, 0);

	appInfo.pEngineName = "CatRenderEngine";
	appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0, 0);
	appInfo.apiVersion = apiVersion;

	//��ȡ��Ӧ����չExtension
	auto extension = getRequiredExtensions();

	//����һ���й���debug��Ϣ�Ľṹ��
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};


	VkInstanceCreateInfo CreateInfo{};//instance ������Ϣ
	CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	CreateInfo.enabledExtensionCount = static_cast<uint32_t>(extension.size());
	CreateInfo.ppEnabledExtensionNames = extension.data();
	if (enableValidationLayers) {
		CreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		CreateInfo.ppEnabledLayerNames = validationLayers.data();

		// ��������ʹ��Ϣ�������ӵ� pNext �ֶ�,��֤��ִ�����vkCreateInstance֮���ܹ�ִ����Ϣ����
		populateDebugMessengerCreateInfo(debugCreateInfo);
		CreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else 
	{
		CreateInfo.enabledLayerCount = 0;
		CreateInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&CreateInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}
}

std::vector<const char*> VulkanCore::getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount); //�����൱�ڻ�ȡ��glfw����չ

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

bool VulkanCore::checkValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);//���ȱ�����ȡ���豸��Ӧ��validation��
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) { //�鿴�Ƿ��ж�Ӧ��validation��
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return false;
}

void VulkanCore::setupDebugMessenger() {
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populateDebugMessengerCreateInfo(createInfo);
    createInfo.pUserData = this;  // ���� this ָ���Ա�ص��з��ʳ�Ա

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (!func || func(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("Failed to set up debug messenger!");
    }
}