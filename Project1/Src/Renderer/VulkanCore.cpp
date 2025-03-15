#define STB_IMAGE_IMPLEMENTATION
#include"Renderer/VulkanCore.h"

#include <set>
#include <chrono>
#include <unordered_map>
#include <cstdlib>
#include <glm/gtx/string_cast.hpp>
#include "Gui/GUIManager.h"
#include <array>

VulkanCore::VulkanCore()
{
	currentFrame = 0;
	camera = new Camera();
	modelManager = std::make_unique<ModelManager>();
	bufferManager = std::make_unique<BufferManager>(device, physicalDevice, commandPool, graphicsQueue);
	materialManager = std::make_unique<MaterialManager>(*bufferManager);
}

VulkanCore::~VulkanCore()
{
	bufferManager.reset();
	delete camera;
	vkDeviceWaitIdle(device);

	// 0. 确保设备操作完成
	if (device != VK_NULL_HANDLE) {
		vkDeviceWaitIdle(device);
	}

	// 1. 销毁同步对象
	for (auto& semaphore : imageAvailableSemaphores) {
		vkDestroySemaphore(device, semaphore, nullptr);
	}
	for (auto& semaphore : renderFinishedSemaphores) {
		vkDestroySemaphore(device, semaphore, nullptr);
	}
	for (auto& fence : inFlightFences) {
		vkDestroyFence(device, fence, nullptr);
	}

	// 2. 销毁命令缓冲区和命令池
	if (!commandBuffers.empty() && commandPool != VK_NULL_HANDLE) {
		vkFreeCommandBuffers(device, commandPool,
			static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
	}
	if (commandPool != VK_NULL_HANDLE) {
		vkDestroyCommandPool(device, commandPool, nullptr);
	}

	// 3. 销毁描述符池和布局
	if (descriptorPool != VK_NULL_HANDLE) {
		vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	}
	if (descriptorSetLayout != VK_NULL_HANDLE) {
		vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
	}

	// 4. 销毁交换链相关资源
	for (auto& framebuffer : SwapChainFramebuffers) {
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}
	for (auto& imageView : swapChainImageViews) {
		vkDestroyImageView(device, imageView, nullptr);
	}
	if (swapChain != VK_NULL_HANDLE) {
		vkDestroySwapchainKHR(device, swapChain, nullptr);
	}

	// 5. 销毁深度缓冲资源
	if (depthImageView != VK_NULL_HANDLE) {
		vkDestroyImageView(device, depthImageView, nullptr);
	}
	if (depthImage != VK_NULL_HANDLE) {
		vkDestroyImage(device, depthImage, nullptr);
	}
	if (depthImageMemory != VK_NULL_HANDLE) {
		vkFreeMemory(device, depthImageMemory, nullptr);
	}

	// 6. 销毁图形管线相关
	if (graphicsPipeline != VK_NULL_HANDLE) {
		vkDestroyPipeline(device, graphicsPipeline, nullptr);
	}
	if (pipelineLayout != VK_NULL_HANDLE) {
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	}
	if (renderPass != VK_NULL_HANDLE) {
		vkDestroyRenderPass(device, renderPass, nullptr);
	}

	// 7. 销毁纹理资源
	if (textureSampler != VK_NULL_HANDLE) {
		vkDestroySampler(device, textureSampler, nullptr);
	}
	if (textureImageView != VK_NULL_HANDLE) {
		vkDestroyImageView(device, textureImageView, nullptr);
	}
	if (textureImage != VK_NULL_HANDLE) {
		vkDestroyImage(device, textureImage, nullptr);
	}
	if (textureImageMemory != VK_NULL_HANDLE) {
		vkFreeMemory(device, textureImageMemory, nullptr);
	}

	// 8. 销毁顶点/索引/统一缓冲区
	if (vertexBuffer != VK_NULL_HANDLE) {
		vkDestroyBuffer(device, vertexBuffer, nullptr);
	}
	if (vertexBufferMemory != VK_NULL_HANDLE) {
		vkFreeMemory(device, vertexBufferMemory, nullptr);
	}
	if (indexBuffer != VK_NULL_HANDLE) {
		vkDestroyBuffer(device, indexBuffer, nullptr);
	}
	if (indexBufferMemory != VK_NULL_HANDLE) {
		vkFreeMemory(device, indexBufferMemory, nullptr);
	}
	for (size_t i = 0; i < uniformBuffers.size(); i++) {
		vkDestroyBuffer(device, uniformBuffers[i], nullptr);
		vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
	}
	for (size_t i = 0; i < dynamic_uniformBuffers.size(); i++) {
		vkDestroyBuffer(device, dynamic_uniformBuffers[i], nullptr);
		vkFreeMemory(device, dynamic_uniformBuffersMemory[i], nullptr);
	}

	// 9. 销毁逻辑设备
	if (device != VK_NULL_HANDLE) {
		vkDestroyDevice(device, nullptr);
	}

	// 10. 销毁Surface和调试信使
	if (surface != VK_NULL_HANDLE) {
		vkDestroySurfaceKHR(instance, surface, nullptr);
	}
	if (debugMessenger != VK_NULL_HANDLE) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)
			vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func) {
			func(instance, debugMessenger, nullptr);
		}
	}



}

void VulkanCore::initVulkan(GLFWwindow* window, GUIManager* m_GUIManager)
{
	this->window = window;
	this->m_GUIManager = m_GUIManager;
	
	camera->initCamera();
	camera->setTranslation(glm::vec3(0.0f, 0, -5));
	

	createInstance();
	setupDebugMessenger();
	createSurface(this->window);
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	createRenderPass();



	createDescriptorSetLayout();
	createDescriptorPool();
	
	createGraphicsPipeline_Rasterizer();

	createTextureImage();
	//createTextureImageView();
	//createTextureSampler();

	createUniformBuffers();
	createDynamicUniformBuffers();

	createDescriptorSets();

	createCommandPool();
	createDepthResources();
	createFramebuffers();




	loadModel();
	createVertexBuffer();
	createIndexBuffer();


	createCommandBuffers();
	createSyncObjects();
	
}

void VulkanCore::drawFrame()
{
	//finish one frame
	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
	//Acquiring an image from the swap chain avoid deadlock
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}
	//Acquiring an image from the swap chain

	vkResetFences(device, 1, &inFlightFences[currentFrame]);
	//finish one frame

	//Acquiring an image from the swap chain

	//recording the command buffer
	vkResetCommandBuffer(commandBuffers[currentFrame], 0);
	recordCommandBuffer(commandBuffers[currentFrame], imageIndex);
	//recording the command buffer
	//Acquiring an image from the swap chain

	//update uniformbuffer
	updateUniformBuffer(currentFrame);

	updateUniformBuffer_dynamic(currentFrame);
	//update uniformbuffer

	//Submitting the command buffer
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;
	VK_CHECK (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]))
	//Submitting the command buffer

	//Presentation
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr; // Optional
	result = vkQueuePresentKHR(presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		framebufferResized = false;
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

float VulkanCore::getAspectRatio()
{
	return swapChainExtent.width / static_cast<float>(swapChainExtent.height);
}

 void VulkanCore::createInstance()
{
	if (enableValidationLayers && !checkValidationLayerSupport()) {
		throw std::runtime_error("validation layers requested, but not available!");
	}

	uint32_t apiVersion = VK_VERSION_1_1;
	if (vkEnumerateInstanceVersion(&apiVersion) != VK_SUCCESS) {
		throw std::runtime_error("Failed to query the highest supported Vulkan version.");
	}//遍历设备支持的最大版本号

	uint32_t major = VK_VERSION_MAJOR(apiVersion);
	uint32_t minor = VK_VERSION_MINOR(apiVersion);
	uint32_t patch = VK_VERSION_PATCH(apiVersion);
	std::cout << "Max Vulkan Version: " << major << "." << minor << "." << patch << "\n";

	VkApplicationInfo appInfo{};//创建app信息，包括版本号版本名称等等
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "CatRender";
	appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);

	appInfo.pEngineName = "CatRenderEngine";
	appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
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

	VK_CHECK(vkCreateInstance(&CreateInfo, nullptr, &instance))
}

 void VulkanCore::createSurface(GLFWwindow* window)
{
	VK_CHECK(glfwCreateWindowSurface(instance, window, nullptr, &surface));//创建一个窗口表面
}

 void VulkanCore::pickPhysicalDevice()
{
	uint32_t physicalDeciveNum = 0;
	vkEnumeratePhysicalDevices(instance, &physicalDeciveNum, NULL);
	std::vector<VkPhysicalDevice> physicalDevices(physicalDeciveNum);
	vkEnumeratePhysicalDevices(instance, &physicalDeciveNum, physicalDevices.data());//遍历所有的物理设备
	for (const auto& device : physicalDevices) {
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("failed to find a suitable GPU!");
	}

}

 void VulkanCore::createLogicalDevice()
{
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);//首先要遍历选择好的物理设备的队列族，来创建对应的逻辑设备
	VkDeviceQueueCreateInfo queueCreateInfo{};

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = {
		indices.graphicsFamily.value(),
		indices.presentFamily.value()
	};

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {//遍历所有的队列族,并且创建对应的队列族,
		VkDeviceQueueCreateInfo queueCreateInfo{};    //将队列族的信息都存到queueCreateInfos中，最后一起创建
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};//创建一个设备特性的结构体
	deviceFeatures.samplerAnisotropy = VK_TRUE;//这里可以设置一些设备特性，比如采样器的各种特性

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();//将队列族的信息都存到queueCreateInfos中，最后一起创建

	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}  //这里是设置一些验证层的信息，这是创建逻辑设备的时候的一些验证信息
	else {
		createInfo.enabledLayerCount = 0;
	}

	VK_CHECK(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device))//这里创建逻辑设备，如果失败则抛出异常



	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);//获取逻辑设备中对应的队列族，存住在graphicsQueue中
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);//同上


}

 void VulkanCore::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);//首先查询交换链的支持情况
	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);//选择交换链的表面格式
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);//选择交换链的展示模式
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);//选择交换链的范围
	
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}//这里是设置交换链中的图像数量,确保比最小交换图像数量多一个，提供一个缓冲区。

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1; // 交换链中的图像数组层数
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;// 交换链中图像的使用方式,这里是颜色附件
	
	//接下来填写关于交换链的队列族的信息，因为交换链和队列相关，在每一个交换的图像上都会绑定一个指令，而指令和队列相关。
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;//如果不是同一个队列族，就是并发模式
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;//如果是同一个队列族，就是独占模式
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;//设置交换链的转换
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;//设置交换链的alpha通道
	createInfo.presentMode = presentMode;//设置交换链的展示模式
	createInfo.clipped = VK_TRUE;//设置交换链的裁剪

	VK_CHECK(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain));//创建交换链

	//记录下交换链的detail信息，包括交换链的图像，格式，范围，便于后续的使用。
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);//获取交换链中的图像
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());//将交换链中的图像存到swapChainImages中
	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

 void VulkanCore::createImageViews()
{
	//创建完swapchain之后，我们需要创建对应的imageview。因为swapchin中的图像不能直接用来渲染，需要创建对应的imageview
	swapChainImageViews.resize(swapChainImages.size());

	for (uint32_t i = 0; i < swapChainImages.size(); i++) {
		swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}

 void VulkanCore::createRenderPass()
{

	std::vector<VkAttachmentDescription> attachments = {};
	std::vector<VkSubpassDescription> subpasses = {};
	std::vector<VkSubpassDependency> dependencies = {};


	/*
	* typedef struct VkRenderingInfo {
	*	VkStructureType                     sType;
	*	const void*                         pNext;
	*	VkRenderingFlags                    flags;
	*	VkRect2D                            renderArea;
	*	uint32_t                            layerCount;
	*	uint32_t                            viewMask;
	*	uint32_t                            colorAttachmentCount;
	*	const VkRenderingAttachmentInfo*    pColorAttachments;
	*	const VkRenderingAttachmentInfo*    pDepthAttachment;
	*	const VkRenderingAttachmentInfo*    pStencilAttachment;
	} VkRenderingInfo;
	*/
	//首先填充关于用于显示的颜色附件的信息
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChainImageFormat; // 附件的格式（与交换链图像格式一致）
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // 采样数（无多重采样）
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // 加载操作：清除附件
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // 存储操作：保存附件内容
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // 模板加载操作：不关心
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // 模板存储操作：不关心
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // 初始布局：未定义
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // 最终布局：用于呈现颜色附件索引
	attachments.push_back(colorAttachment);

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//填充关于深度附件的信息
	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = findDepthFormat(); // 深度附件的格式
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // 采样数（无多重采样）
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // 加载操作：清除附件
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // 存储操作：不关心
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // 模板加载操作：不关心
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // 模板存储操作：不关心
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // 初始布局：未定义
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; // 最终布局：深度附件深度附件索引
	attachments.push_back(depthAttachment);

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	//填充关于子通道的信息
	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // 绑定点：图形管线
	subpass.colorAttachmentCount = 1; // 颜色附件数量
	subpass.pColorAttachments = &colorAttachmentRef; // 颜色附件引用
	subpass.pDepthStencilAttachment = &depthAttachmentRef; // 深度附件引用
	subpasses.push_back(subpass);

	//填充关于依赖的信息
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL; // 外部子通道,因为这里是第一个子通道
	dependency.dstSubpass = 0; // 目标子通道,因为有且仅有一个子通道
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // 源阶段：颜色附件输出
	dependency.srcAccessMask = 0; // 源访问掩码：无
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // 目标阶段：颜色附件输出
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; // 目标访问掩码：颜色附件写入
	dependencies.push_back(dependency);

	//-----------------------------------------------------------------------------------------------------------

	//用于imgui的color attachment
	VkAttachmentDescription colorAttachment_ImGui{};
	colorAttachment_ImGui.format = swapChainImageFormat; // 附件的格式（与交换链图像格式一致）
	colorAttachment_ImGui.samples = VK_SAMPLE_COUNT_1_BIT; // 采样数（无多重采样）
	colorAttachment_ImGui.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD; // 加载操作：清除附件
	colorAttachment_ImGui.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // 存储操作：保存附件内容
	colorAttachment_ImGui.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // 模板加载操作：不关心
	colorAttachment_ImGui.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; // 模板存储操作：不关心
	colorAttachment_ImGui.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL; // 初始布局：
	colorAttachment_ImGui.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // 最终布局：用于呈现
	attachments.push_back(colorAttachment_ImGui);

	VkAttachmentReference colorAttachmentRef_ImGui{};
	colorAttachmentRef_ImGui.attachment = 2;
	colorAttachmentRef_ImGui.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass_ImGui{};
	subpass_ImGui.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // 绑定点：图形管线
	subpass_ImGui.colorAttachmentCount = 1; // 颜色附件数量
	subpass_ImGui.pColorAttachments = &colorAttachmentRef_ImGui; // 颜色附件引用
	subpasses.push_back(subpass_ImGui);

	VkSubpassDependency dependency_ImGui{};
	dependency_ImGui.srcSubpass = 0; // 0号通道用于渲染场景
	dependency_ImGui.dstSubpass = 1; // 目标子通道,是下一个子通道
	dependency_ImGui.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // 源阶段：颜色附件输出
	dependency_ImGui.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; // 源访问掩码：颜色附件写入
	dependency_ImGui.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // 目标阶段：颜色附件输出
	dependency_ImGui.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT; // 目标访问掩码：颜色附件读取
	dependencies.push_back(dependency_ImGui);

	//-------------------------------------------------------------------------------------------------

	//填充渲染通道信息。最后，将所有配置传递给 VkRenderPassCreateInfo 并创建 Render Pass。
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = static_cast<uint32_t>(subpasses.size());	
	renderPassInfo.pSubpasses = subpasses.data();
	renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	renderPassInfo.pDependencies = dependencies.data();



	VK_CHECK(vkCreateRenderPass(device, &renderPassInfo, NULL, &renderPass))
}

 void VulkanCore::createDescriptorSetLayout()
{
	 //TODO:创建一个用于material特征数的UBO以及一个用于采样器的描述符集布局
	//创建一个描述符集布局。
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding dynamic_uboLayoutBinding{};
	dynamic_uboLayoutBinding.binding = 1;
	dynamic_uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	dynamic_uboLayoutBinding.descriptorCount = 1;
	dynamic_uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	//将两个描述符集布局绑定到一个数组中。
	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, dynamic_uboLayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	VK_CHECK(vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout))

	//创建一个采样绑定的描述符集布局。
	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding =0;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo TextureLayoutInfo{};
	TextureLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	TextureLayoutInfo.bindingCount = 1;
	TextureLayoutInfo.pBindings = &samplerLayoutBinding;
	VK_CHECK(vkCreateDescriptorSetLayout(device, &TextureLayoutInfo, nullptr, &materialSetLayout))

	VkDescriptorSetLayoutBinding MaterialUboLayoutBinding{};
	MaterialUboLayoutBinding.binding = 0;
	MaterialUboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	MaterialUboLayoutBinding.descriptorCount = 1;
	MaterialUboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo MaterialUboLayout{};
	MaterialUboLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	MaterialUboLayout.bindingCount = 1;
	MaterialUboLayout.pBindings = &MaterialUboLayoutBinding;
	VK_CHECK(vkCreateDescriptorSetLayout(device, &MaterialUboLayout, nullptr, &materialParaSetLayout))

	layouts_ = { descriptorSetLayout, materialSetLayout, materialParaSetLayout };
}

 void VulkanCore::createDescriptorPool()
 {
	 std::array<VkDescriptorPoolSize, 2> poolSizes{};
	 poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	 poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	 poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	 poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);



	 VkDescriptorPoolCreateInfo poolInfo{};
	 poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	 poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	 poolInfo.pPoolSizes = poolSizes.data();
	 poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 2);

	 VK_CHECK(vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool))


		 //创建和Texture相关的描述符池
	std::array<VkDescriptorPoolSize, 2> TexturePoolSizes{};
	TexturePoolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	TexturePoolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * TEXTURE_NUM);

	TexturePoolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	TexturePoolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * TEXTURE_NUM);

	VkDescriptorPoolCreateInfo TexturePoolInfo{};
	TexturePoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	TexturePoolInfo.poolSizeCount = static_cast<uint32_t>(TexturePoolSizes.size());
	TexturePoolInfo.pPoolSizes = TexturePoolSizes.data();
	TexturePoolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 2 * TEXTURE_NUM);
	VK_CHECK(vkCreateDescriptorPool(device, &TexturePoolInfo, nullptr, &TextureDescriptorPool))


 }

 void VulkanCore::createUniformBuffers()
 {
	 //TODO:将管理UBO集成到bufferManager中

	 VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	 uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	 uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
	 uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	 for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		 createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
		 vkMapMemory(device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
	 }
 }

 void VulkanCore::createDynamicUniformBuffers()
 {
	
	 VkPhysicalDeviceProperties deviceProperties;
	 vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
	 size_t minUboAlignment = deviceProperties.limits.minUniformBufferOffsetAlignment;//获取最小位移单位
	 dynamicAlignment = sizeof(UniformBufferObject);//获取uniformbuffer的大小
	 if (minUboAlignment > 0) {
		 dynamicAlignment = (dynamicAlignment + minUboAlignment - 1) & ~(minUboAlignment - 1);//保证偏移量是2^n
	 }

	 modelManager->setOffest(static_cast<uint32_t> (dynamicAlignment)); //设定modelmanager的dynamicbuffer的offset；

	 size_t bufferSize = dynamicAlignment * MAX_NUM_OBJECT;

	 dynamic_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	 dynamic_uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
	 dynamic_uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);
	 for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		 createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, dynamic_uniformBuffers[i], dynamic_uniformBuffersMemory[i]);
		 vkMapMemory(device, dynamic_uniformBuffersMemory[i], 0, bufferSize, 0, &dynamic_uniformBuffersMapped[i]);
	 }

	 //TODO:创建材质的特征值ubo
	 Texture_dynamicAlignment = sizeof(MaterialManager::MaterialBlock);//获取uniformbuffer的大小
	 if (minUboAlignment > 0) {
		 Texture_dynamicAlignment = (Texture_dynamicAlignment + minUboAlignment - 1) & ~(minUboAlignment - 1);//保证偏移量是2^n
	 }

	 materialManager->setOffest(static_cast<uint32_t>(Texture_dynamicAlignment)); //设定modelmanager的dynamicbuffer的offset；

	 bufferSize = Texture_dynamicAlignment * TEXTURE_NUM;

	 Texture_dynamic_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	 Texture_dynamic_uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
	 Texture_dynamic_uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);
	 for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		 createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, Texture_dynamic_uniformBuffers[i], Texture_dynamic_uniformBuffersMemory[i]);
		 vkMapMemory(device, Texture_dynamic_uniformBuffersMemory[i], 0, bufferSize, 0, &Texture_dynamic_uniformBuffersMapped[i]);
	 }

 }

 void VulkanCore::createDescriptorSets()
 {

	 //创建DescriptorSets指定了将要绑定到描述符的实际缓冲区或图像资源
	 VkDescriptorSetAllocateInfo allocInfo{};
	 allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	 allocInfo.descriptorPool = descriptorPool;
	 allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	 allocInfo.pSetLayouts = &descriptorSetLayout;
	 descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	 VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()))

	 VkDescriptorSetAllocateInfo TextureUBOAllocInfo{};
	 TextureUBOAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	 TextureUBOAllocInfo.descriptorPool = TextureDescriptorPool;
	 TextureUBOAllocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	 TextureUBOAllocInfo.pSetLayouts = &materialParaSetLayout;
	 TextureUBODescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	 VK_CHECK(vkAllocateDescriptorSets(device, &TextureUBOAllocInfo, TextureUBODescriptorSets.data()))

	 for (auto& [MaterialID, MaterialViewer] : materialManager->getMaterialViewers()) {
		 VkDescriptorSetAllocateInfo TextureAllocInfo{};
		 TextureAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		 TextureAllocInfo.descriptorPool = TextureDescriptorPool;
		 TextureAllocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
		 TextureAllocInfo.pSetLayouts = &materialSetLayout;
		 TextureDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
		 VK_CHECK(vkAllocateDescriptorSets(device, &TextureAllocInfo, TextureDescriptorSets.data()))
	 	 MaterialViewer->setDescriptorSets(TextureDescriptorSets);
	 }




	 for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		 VkDescriptorBufferInfo bufferInfo{};
		 bufferInfo.buffer = uniformBuffers[i];
		 bufferInfo.offset = 0;
		 bufferInfo.range = sizeof(UniformBufferObject);

		 VkDescriptorImageInfo imageInfo{};
		 imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		 imageInfo.imageView = textureImageView;
		 imageInfo.sampler = textureSampler;

		 VkDescriptorBufferInfo dynamic_bufferInfo{};
		 dynamic_bufferInfo.buffer = dynamic_uniformBuffers[i];
		 dynamic_bufferInfo.offset = 0;
		 dynamic_bufferInfo.range = modelManager->getOffeset();//是单段大小

		 std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

		 descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		 descriptorWrites[0].dstSet = descriptorSets[i];
		 descriptorWrites[0].dstBinding = 0;
		 descriptorWrites[0].dstArrayElement = 0;
		 descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		 descriptorWrites[0].descriptorCount = 1;
		 descriptorWrites[0].pBufferInfo = &bufferInfo;

		 descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		 descriptorWrites[1].dstSet = descriptorSets[i];
		 descriptorWrites[1].dstBinding = 1;
		 descriptorWrites[1].dstArrayElement = 0;
		 descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		 descriptorWrites[1].descriptorCount = 1;
		 descriptorWrites[1].pBufferInfo = &dynamic_bufferInfo;

		 vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

		 //上传材质的UBO以及材质的imageviewer
		 VkDescriptorBufferInfo Texture_dynamic_bufferInfo{};
		 dynamic_bufferInfo.buffer = Texture_dynamic_uniformBuffers[i];
		 dynamic_bufferInfo.offset = 0;
		 dynamic_bufferInfo.range = modelManager->getOffeset();//是单段大小

	 	VkWriteDescriptorSet TextureUBOWrites{};
		 TextureUBOWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		 TextureUBOWrites.dstSet = TextureDescriptorSets[i];
		 TextureUBOWrites.dstBinding = 0;
		 TextureUBOWrites.dstArrayElement = 0;
		 TextureUBOWrites.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		 TextureUBOWrites.descriptorCount = 1;
		 TextureUBOWrites.pBufferInfo = &Texture_dynamic_bufferInfo;
		 vkUpdateDescriptorSets(device, 1, &TextureUBOWrites, 0, nullptr);

		 for (auto& [MaterialID, MaterialViewer] : materialManager->getMaterialViewers()) {
			 //TODO:将所有需要创建布局的材质布局都update
			 VkWriteDescriptorSet TextureDescriptorWrites{};
			 TextureDescriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			 TextureDescriptorWrites.dstSet = MaterialViewer->getDescriptorSet(i);
			 TextureDescriptorWrites.dstBinding = 1;
			 TextureDescriptorWrites.dstArrayElement = 0;
			 TextureDescriptorWrites.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			 TextureDescriptorWrites.descriptorCount = 1;
			 TextureDescriptorWrites.pImageInfo = &imageInfo;
			 vkUpdateDescriptorSets(device, 1, &TextureDescriptorWrites, 0, nullptr);
		 }
	 }
 }



 void VulkanCore::createGraphicsPipeline_Rasterizer()
{

	//load shader module
	auto vertShaderCode = readFile("shaders/vert.spv");
	auto fragShaderCode = readFile("shaders/frag.spv");
	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	//vertex input
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	auto bindingDescription = Mesh::getBindingDescription();
	auto attributeDescriptions = Mesh::getAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();//bing some feature

	//Input assembly输入装配阶段
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	//Input assembly

	//Viewports and scissors
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapChainExtent.width);
	viewport.height = static_cast<float>(swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;
	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;
	//Viewports and scissors

	//Rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;//深度裁剪
	rasterizer.rasterizerDiscardEnable = VK_FALSE;//光栅化丢弃
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;//多边形模式
	rasterizer.lineWidth = 1.0f;//线宽
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
	//Rasterizer

	rasterizer.cullMode = VK_CULL_MODE_NONE;
	//rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;//修改绘制顺序，变为顺时针，这样的话就不会被剔除掉了，因为前面的投影矩阵使用了z轴翻转

	//Multisampling
	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional
	//Multisampling

	//Color blending
	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; // Optional 混合常量
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional
	//Color blending

	//dynamic stage
	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};//动态状态

	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();


	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layouts_.size());
	pipelineLayoutInfo.pSetLayouts = layouts_.data();

	VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout))

	//depth stencil
	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional

	//CreateGraphicsPipeline
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline))

	vkDestroyShaderModule(device, fragShaderModule, nullptr);
	vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

 void VulkanCore::createCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	VK_CHECK(vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool));
}

 void VulkanCore::createFramebuffers()
{
	//初始化framebuffer的大小，一个framebuffer对应一个swapchain的imageview
	SwapChainFramebuffers.resize(swapChainImageViews.size());
	for (size_t i = 0; i < SwapChainFramebuffers.size(); i++) { //绑定对应的imageview和depthimageview的信息,以后可能会绑定更多的attachments
		std::array<VkImageView, 3> attachments = {
			swapChainImageViews[i],
			depthImageView,
			swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		VK_CHECK(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &SwapChainFramebuffers[i]));
	}
}

 void VulkanCore::createTextureImage()
{
	//int texWidth, texHeight, texChannels;//纹理图片的信息读取
	//stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	//VkDeviceSize imageSize = static_cast<VkDeviceSize>(texWidth) * texHeight * 4;
	//mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
	//if (!pixels) {
	//	throw std::runtime_error("failed to load texture image!");
	//}

	//VkBuffer stagingBuffer;
	//VkDeviceMemory stagingBufferMemory;
	//createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	//void* data;
	//vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
	//memcpy(data, pixels, static_cast<size_t>(imageSize));
	//vkUnmapMemory(device, stagingBufferMemory);
	//stbi_image_free(pixels);

	//createImage(texWidth, texHeight, mipLevels, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,VK_IMAGE_USAGE_TRANSFER_SRC_BIT | 
	//	VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
	//	textureImage, textureImageMemory);
	//transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, 
	//	VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
	//copyBufferToImage(stagingBuffer, textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	////
	////transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels);

	//generateMipmaps(textureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, mipLevels);//生成贴图对应的mipmap

	//vkDestroyBuffer(device, stagingBuffer, nullptr);
	//vkFreeMemory(device, stagingBufferMemory, nullptr);

	 for (auto path : TEXTURE_PATH) {
		 materialManager->loadTextureImage(path);
	 }
}

 void VulkanCore::createTextureImageView()
{
	textureImageView = createImageView(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
}

 void VulkanCore::createTextureSampler()
{
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;//放大过滤器
	samplerInfo.minFilter = VK_FILTER_LINEAR;//缩小过滤器
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;//U轴的寻址模式
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;//V轴的寻址模式
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;//W轴的寻址模式
	samplerInfo.anisotropyEnable = VK_TRUE;//各向异性过滤

	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;//边界颜色
	samplerInfo.unnormalizedCoordinates = VK_FALSE;//不归一化坐标轴
	samplerInfo.compareEnable = VK_FALSE;//禁用比较功能（VK_FALSE）。比较功能用于深度纹理采样时，比较采样值和参考值（通常用于阴影贴图等）。
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS; //设置比较操作为始终通过
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR; //设置多级渐远纹理的过滤模式为线性过滤
	samplerInfo.maxLod = VK_LOD_CLAMP_NONE; // 不限制最大细节层级

	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(physicalDevice, &properties);//根据设备来设置采样器的一些数值
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

	VK_CHECK(vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler))
}

 void VulkanCore::loadModel()
{
	 modelManager->LoadMeshs(PATHS);
	for (int i=0;i<5;i++)
	{
		modelManager->createModelInstance(0);
	}
	for (int i = 0; i < 5; i++)
	{
		modelManager->createModelInstance(1);
	}

}

 void VulkanCore::createVertexBuffer()
{
	 for (auto& [id, mesh] : *modelManager->getMeshs())
	 {
		 bufferManager->createVertexBuffer(mesh.get());
	 }
	//VkBuffer stagingBuffer;
	//VkDeviceMemory stagingBufferMemory;
	//VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
	//createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	//	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
	////VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT表示这个buffer的数据可以被cpu读取

	////将数据拷贝到buffer中
	//void* data;
	//vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	//memcpy(data, vertices.data(), (size_t)bufferSize);
	//vkUnmapMemory(device, stagingBufferMemory);

	////创建一个device local的buffer，只有gpu能访问到
	//createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
	//
	//copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	////写入完毕，删除stagingBuffer
	//vkDestroyBuffer(device, stagingBuffer, nullptr);
	//vkFreeMemory(device, stagingBufferMemory, nullptr);

}

 void VulkanCore::createIndexBuffer()
{
	 for (auto& [id, mesh] : *modelManager->getMeshs())
	 {
		 bufferManager->createIndexBuffer(mesh.get());
	 }
	////同vertexbuffer
	//VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
	//VkBuffer stagingBuffer;
	//VkDeviceMemory stagingBufferMemory;
	//createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	//	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
	//void* data;
	//vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	//memcpy(data, indices.data(), (size_t)bufferSize);
	//vkUnmapMemory(device, stagingBufferMemory);

	//createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
	//	VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	//copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	//vkDestroyBuffer(device, stagingBuffer, nullptr);
	//vkFreeMemory(device, stagingBufferMemory, nullptr);
}


void VulkanCore::createCommandBuffers()
{
	//创建commandbuffer来存储后面与swapchain绑定的command
	commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	VK_CHECK(vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()));
}

void VulkanCore::createSyncObjects()
{
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);//创建对应的信号量和栅栏量。
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {

			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

 void VulkanCore::createDepthResources()
{
	VkFormat depthFormat = findDepthFormat();
	createImage(swapChainExtent.width, swapChainExtent.height, 1, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
	depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
	transitionImageLayout(depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
}


//-------------------下面的函数是用于填充信息以及验证需求的函数------------------------------------------

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

	return true;
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

 bool VulkanCore::isDeviceSuitable(VkPhysicalDevice device)
{
	return true;
}

 VulkanCore::QueueFamilyIndices VulkanCore::findQueueFamilies(VkPhysicalDevice device)
{
	VulkanCore::QueueFamilyIndices indices;
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());//找到设备对应的队列族

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {//查看是否支持对应的队列族
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);//查看是否支持对应的surface

		if (presentSupport) {
			indices.presentFamily = i;
		}
		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

 VulkanCore::SwapChainSupportDetails VulkanCore::querySwapChainSupport(VkPhysicalDevice const device) const {
	SwapChainSupportDetails details;

	//首先查询交换链的表面能力
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
	//查询交换链的格式
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	//如果有格式的话，就将格式存到details中
	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}
	//查询交换链的展示模式
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
	//如果有展示模式的话，就将展示模式存到details中
	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;

}

 VkSurfaceFormatKHR VulkanCore::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	return availableFormats[0];
}

 VkPresentModeKHR VulkanCore::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}
	return VK_PRESENT_MODE_FIFO_KHR;
}

 VkExtent2D VulkanCore::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
	if (capabilities.currentExtent.width != UINT32_MAX) { //如果当前的范围是无效的，就返回当前的范围
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};
		actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
		return actualExtent;
	}
}

 VkImageView VulkanCore::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) const
{
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = mipLevels;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.image = image;

	VkImageView imageView;
	VK_CHECK(vkCreateImageView(device, &viewInfo, nullptr, &imageView));

	return imageView;
}

 VkShaderModule VulkanCore::createShaderModule(const std::vector<char>& code) const
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}
	return shaderModule;
}

void VulkanCore::createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format,
	VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
	//创建一个image
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = mipLevels;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;


	VK_CHECK(vkCreateImage(device, &imageInfo, nullptr, &image));
	//为image分配内存
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, image, &memRequirements);

	//分配内存
	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory))
	VK_CHECK(vkBindImageMemory(device, image, imageMemory, 0))

}

uint32_t VulkanCore::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void VulkanCore::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout; // 源布局
	barrier.newLayout = newLayout; // 目标布局
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; // 不转移队列族所有权
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; // 不转移队列族所有权
	barrier.image = image; // 要转换布局的图像
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; // 颜色方面
	barrier.subresourceRange.levelCount = mipLevels; // 第 mipLevels 级 Mipmap
	barrier.subresourceRange.baseArrayLayer = 0; // 第 0 层数组层
	barrier.subresourceRange.layerCount = 1; // 1 层数组层

	barrier.srcAccessMask = 0; // TODO
	barrier.dstAccessMask = 0; // TODO👇
	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	//下面是要转换的类型
	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (hasStencilComponent(format)) {
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}
	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);
	endSingleTimeCommands(commandBuffer);
}

VkCommandBuffer VulkanCore::beginSingleTimeCommands() const {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;
	//分配一个commandbuffer
	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void VulkanCore::endSingleTimeCommands(VkCommandBuffer commandBuffer) const {
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

bool VulkanCore::hasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void VulkanCore::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	//1. 创建缓冲区对象
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO; //结构体类型
	bufferInfo.size = size; //缓冲区的大小(字节数)
	bufferInfo.usage = usage; //缓冲区的用途(如顶点缓冲区、传输缓冲区等)
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; //缓冲区的共享模式(独占模式)

	//调用 Vulkan API 创建缓冲区
	VK_CHECK(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer))

	//2.查询缓冲区的内存需求
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	//3. 分配内存
	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO; // 结构体类型
	allocInfo.allocationSize = memRequirements.size; // 需要分配的内存大小
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties); // 找到合适的内存类型索引

	//调用 Vulkan API 分配内存
	VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory))

	//4. 将内存绑定到缓冲区
	vkBindBufferMemory(device, buffer, bufferMemory, 0); //将分配的内存绑定到缓冲区对象
}

void VulkanCore::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();
	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		width,
		height,
		1
	};

	vkCmdCopyBufferToImage(
		commandBuffer, // 命令缓冲区
		buffer,    // 源缓冲区
		image,     // 目标图像
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, // 目标图像的布局
		1,            // 复制的区域数量
		&region       // 复制的区域
	);
	endSingleTimeCommands(commandBuffer);
}

void VulkanCore::generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
	//检查对线性模糊的检查
	VkFormatProperties formatProperties;
	vkGetPhysicalDeviceFormatProperties(physicalDevice, imageFormat, &formatProperties);
	if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
		throw std::runtime_error("texture image format does not support linear blitting!");
	}
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.image = image;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;


	int32_t mipWidth = texWidth;
	int32_t mipHeight = texHeight;

	for (uint32_t i = 1; i < mipLevels; i++) {
		barrier.subresourceRange.baseMipLevel = i - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		VkImageBlit blit{};
		blit.srcOffsets[0] = { 0, 0, 0 };
		blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
		blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.srcSubresource.mipLevel = i - 1;
		blit.srcSubresource.baseArrayLayer = 0;
		blit.srcSubresource.layerCount = 1;

		blit.dstOffsets[0] = { 0, 0, 0 };
		blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
		blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		blit.dstSubresource.mipLevel = i;
		blit.dstSubresource.baseArrayLayer = 0;
		blit.dstSubresource.layerCount = 1;
		vkCmdBlitImage(commandBuffer,
			image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &blit,
			VK_FILTER_LINEAR);

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);
		if (mipWidth > 1) mipWidth /= 2;
		if (mipHeight > 1) mipHeight /= 2;
	}
	barrier.subresourceRange.baseMipLevel = mipLevels - 1;
	barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	vkCmdPipelineBarrier(commandBuffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
		0, nullptr,
		0, nullptr,
		1, &barrier);

	endSingleTimeCommands(commandBuffer);
}

void VulkanCore::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer);
}

VkFormat VulkanCore::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const {
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}

	throw std::runtime_error("failed to find supported format!");
}

void VulkanCore::recreateSwapChain()
{
	int width = 0, height = 0;
	glfwGetFramebufferSize(window, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(device);
	cleanupSwapChain();

	createSwapChain();
	createImageViews();
	createDepthResources();
	createFramebuffers();

	float aspect = static_cast<float>(width) / height;
	camera->updateAspectRatio(aspect);
}

void VulkanCore::cleanupSwapChain()
{
	if (depthImageView != VK_NULL_HANDLE)
		vkDestroyImageView(device, depthImageView, nullptr);
	if (depthImage != VK_NULL_HANDLE)
		vkDestroyImage(device, depthImage, nullptr);
	if (depthImageMemory != VK_NULL_HANDLE)
	vkFreeMemory(device, depthImageMemory, nullptr);

	for (auto framebuffer : SwapChainFramebuffers) {
		if (framebuffer != VK_NULL_HANDLE)
			vkDestroyFramebuffer(device, framebuffer, nullptr);
	}

	for (auto imageView : swapChainImageViews) {
		if (imageView != VK_NULL_HANDLE)
			vkDestroyImageView(device, imageView, nullptr);
	}
	if(swapChain!=VK_NULL_HANDLE)
		vkDestroySwapchainKHR(device, swapChain, nullptr);
}

void VulkanCore::recordCommandBuffer(VkCommandBuffer commandBuffer, const uint32_t& imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo))
	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = SwapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapChainExtent;
	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	{
		//1.normal render pass
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapChainExtent.width);
		viewport.height = static_cast<float>(swapChainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapChainExtent;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
		int num = 0;
		for (auto& [meshID, mesh] : *modelManager->getMeshs()) {
			VkBuffer vertexBuffers[] = { bufferManager->getBuffer(mesh->getVertexBufferId()).buffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

			vkCmdBindIndexBuffer(commandBuffer, bufferManager->getBuffer(mesh->getIndexBufferId()).buffer, 0, VK_INDEX_TYPE_UINT32);

			for (uint32_t modelInstanceID : modelManager->getModelBindMesh(meshID)) {
				std::vector<uint32_t> dynamic_uniformOffset = { modelManager->getModelInstanceByID(modelInstanceID)->uniformOffset };

				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, 
					&descriptorSets[currentFrame], 1, dynamic_uniformOffset.data());

				vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1,
					materialManager->getMaterialViewer(num)->getDescriptorSet(currentFrame), 1, dynamic_uniformOffset.data());
				vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(modelManager->getMesh(meshID)->getIndices().size()), 1, 0, 0, 0);
			}

		}//gui render pass
		vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);
		// GUI 新帧
		m_GUIManager->BeginFrame();
		m_GUIManager->EndFrame(commandBuffer);
	}
	vkCmdEndRenderPass(commandBuffer);
	VK_CHECK(vkEndCommandBuffer(commandBuffer))
}

void VulkanCore::updateUniformBuffer(size_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	UniformBufferObject ubo{};

	static auto prevTime = std::chrono::high_resolution_clock::now();
	float deltaTime = std::chrono::duration<float>(currentTime - prevTime).count();
	prevTime = currentTime;

	camera->update(deltaTime); // 先更新摄像头状态

	ubo.view = camera->matrices.view;
	ubo.proj = camera->matrices.perspective;
	memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void VulkanCore::updateUniformBuffer_dynamic(size_t currentImage) const 
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();


	static auto prevTime = std::chrono::high_resolution_clock::now();
	float deltaTime = std::chrono::duration<float>(currentTime - prevTime).count();
	prevTime = currentTime;

	camera->update(deltaTime); // 先更新摄像头状态
	float num = 10.0f;

	for (auto& [modelID, modelInstance ] : modelManager->getModelInstances()) {

		dynamic_UniformBufferObject ubo{};
		//ubo.model = modelInstance->transM;
		modelInstance->transM = glm::translate(glm::mat4(1.0f), glm::vec3(static_cast<float>(sin(num)), glm::cos(num), 0.0f));
		ubo.model = modelInstance->transM;

		char* data = static_cast<char*>(dynamic_uniformBuffersMapped[currentImage]) + modelInstance->uniformOffset;
		memcpy(data, &ubo, sizeof(ubo));
		num += time;
	}
}

VkFormat VulkanCore::findDepthFormat() {
	return findSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

