#pragma once
#include "Window/window.h" 
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstdlib>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class VulkanCore
{
public:
	VulkanCore();
	~VulkanCore();

	void initVulkan();

public:
    
private:
    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

private:
	void createInstance();

private:
	std::vector<const char*> getRequiredExtensions();
	bool checkValidationLayerSupport();

private:
	const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
	};

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
};


