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
	}//遍历设备支持的最大版本号

	std::cout << "Max Vulkan Version is" << apiVersion << "\n";

	VkApplicationInfo appInfo{};//创建app信息，包括版本号版本名称等等
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "CatRender";
	appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0, 0);

	appInfo.pEngineName = "CatRenderEngine";
	appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0, 0);
	appInfo.apiVersion = apiVersion;

	//获取对应的拓展Extension
	auto extension = getRequiredExtensions();

	//创建一个有关于debug信息的结构体
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};


	VkInstanceCreateInfo CreateInfo{};//instance 创建信息
	CreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	CreateInfo.enabledExtensionCount = static_cast<uint32_t>(extension.size());
	CreateInfo.ppEnabledExtensionNames = extension.data();
	if (enableValidationLayers) {
		CreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		CreateInfo.ppEnabledLayerNames = validationLayers.data();

		// 填充调试信使信息，并附加到 pNext 字段,保证在执行完成vkCreateInstance之后能够执行信息盒子
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
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount); //这里相当于获取到glfw的拓展

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

	std::vector<VkLayerProperties> availableLayers(layerCount);//首先遍历获取到设备对应的validation层
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) { //查看是否有对应的validation层
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
    createInfo.pUserData = this;  // 传递 this 指针以便回调中访问成员

    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (!func || func(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("Failed to set up debug messenger!");
    }
}