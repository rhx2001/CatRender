#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE


#include "Window/window.h" 
#include "glm/glm.hpp"
#include "glm/gtx/hash.hpp"
#include "stb_image/stb_image.h"
#include "ResourceManager/ModelInstance.h"
#include <stdexcept>
#include <iostream>
#include <vector>
#include <optional>
#include <fstream>

#include "Camera.hpp"
#include "ResourceManager/BufferManager.h"
#include "ResourceManager/ModelManager.h"


#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
class GUIManager;
const bool enableValidationLayers = true;
#endif

#define VK_CHECK(f) { VkResult res = (f); if (res != VK_SUCCESS) { std::cerr << "Fatal: VkResult is \"" << res << "\" in " << __FILE__ << " at line " << __LINE__ << std::endl; abort(); } }

const std::string TEXTURE_PATH = "textures/viking_room.png";

const std::string MODEL_PATH = "models/viking_room.obj";
std::vector<std::string> paths{MODEL_PATH};



const int MAX_FRAMES_IN_FLIGHT = 2;

const size_t MAX_NUM_OBJECT = 100;

static std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }
    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

class VulkanCore
{
public:
	VulkanCore();
	~VulkanCore();

	void initVulkan(GLFWwindow* window, GUIManager* m_GUIManager);

    void drawFrame();

    VkDevice getDevice() const { return device; };

    VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; };

    VkQueue getGraphicsQueue() const { return graphicsQueue; }

    VkRenderPass getRenderPass() const { return renderPass; }

    size_t getSwapChainImageCount() const { return swapChainImages.size(); }

	VkPipeline getGraphicsPipeline() const { return graphicsPipeline; }

	VkInstance getInstance() const { return instance; }

    float getAspectRatio();

public:

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily; // 图形绘制队列族
        std::optional<uint32_t> presentFamily; // 表面展示队列族

        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};//设备拓展

    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };





    struct UniformBufferObject {
        glm::mat4 view;
        glm::mat4 proj;
    };

    struct dynamic_UniformBufferObject {
        glm::mat4 model;
    };

public:

    Camera* camera;

public:
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);


private:
    GLFWwindow* window = NULL;
    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VkQueue graphicsQueue = VK_NULL_HANDLE;
	VkQueue presentQueue = VK_NULL_HANDLE;

	VkSwapchainKHR swapChain = VK_NULL_HANDLE;
	std::vector<VkImage> swapChainImages = {};
	VkFormat swapChainImageFormat = VK_FORMAT_UNDEFINED;
	VkExtent2D swapChainExtent = { 0, 0 };
	std::vector<VkImageView> swapChainImageViews;

	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;

	VkRenderPass renderPass = VK_NULL_HANDLE;
	VkPipeline graphicsPipeline = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

	std::vector<VkFramebuffer> SwapChainFramebuffers;

	VkImageView depthImageView = VK_NULL_HANDLE;

	VkImage depthImage = VK_NULL_HANDLE;
    VkDeviceMemory depthImageMemory = VK_NULL_HANDLE;

    VkCommandPool commandPool = VK_NULL_HANDLE;

    VkImage textureImage = VK_NULL_HANDLE;
    VkDeviceMemory textureImageMemory = VK_NULL_HANDLE;
    VkImageView textureImageView = VK_NULL_HANDLE;
    VkSampler textureSampler = VK_NULL_HANDLE;

    uint32_t mipLevels = 0;

    VkBuffer vertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;    
    VkBuffer indexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;

    std::vector<VkBuffer> dynamic_uniformBuffers;
    std::vector<VkDeviceMemory> dynamic_uniformBuffersMemory;
    std::vector<void*> dynamic_uniformBuffersMapped;
    size_t dynamicAlignment;//偏移量

    std::vector<modelInstance*> modelInstances;


    std::vector<VkDescriptorSet> descriptorSets;

    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    size_t currentFrame;

    bool framebufferResized = false;

	GUIManager* m_GUIManager = nullptr;

    std::unique_ptr<ModelManager> modelManager;

    std::unique_ptr<BufferManager> bufferManager;


private:
	void createInstance();
    void createSurface(GLFWwindow* window);
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
	void createImageViews();
	void createRenderPass();
    void createDescriptorSetLayout();
    void createGraphicsPipeline_Rasterizer();
    void createCommandPool();
    void createDepthResources();
	void createFramebuffers();
    void createTextureImage();
    void createTextureImageView();
    void createTextureSampler();
    void loadModel();
    void createVertexBuffer();
    void createIndexBuffer();

	void createUniformBuffers();
    void createDynamicUniformBuffers();
    void createModelInstance();

    void createDescriptorPool();
    void createDescriptorSets();
    void createCommandBuffers();
    void createSyncObjects();


private:
	std::vector<const char*> getRequiredExtensions();
	bool checkValidationLayerSupport();

private:
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};  // 初始化结构体为零
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;  // 结构体类型
        createInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |  // 诊断信息
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |  // 警告信息
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;     // 错误信息
        createInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |      // 一般信息
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |   // 验证相关信息
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;   // 性能问题信息
        createInfo.pfnUserCallback = debugCallback;  // 调试回调函数
    }

    void setupDebugMessenger();//因为把创建messenger创建到pnext下的话只会在创建instance时的错误，但是并没有创建VkDebugUtilsMessengerEXT，所以需要单独创建

    bool isDeviceSuitable(VkPhysicalDevice device);//用于选择适合的设备



	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) const;

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels) const;

	VkShaderModule createShaderModule(const std::vector<char>& code) const;

    VkFormat findDepthFormat();

    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, 
        VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

    VkCommandBuffer beginSingleTimeCommands() const;

    void endSingleTimeCommands(VkCommandBuffer commandBuffer) const;

    bool hasStencilComponent(VkFormat format);

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, 
        VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

    void recreateSwapChain();

    void cleanupSwapChain();

    void recordCommandBuffer(VkCommandBuffer commandBuffer, const uint32_t& imageIndex);

    void updateUniformBuffer(size_t currentImage);

    void updateUniformBuffer_dynamic(size_t currentImage) const;


};

