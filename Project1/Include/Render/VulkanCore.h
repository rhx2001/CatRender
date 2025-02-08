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
        createInfo = {};  // ��ʼ���ṹ��Ϊ��
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;  // �ṹ������
        createInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |  // �����Ϣ
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |  // ������Ϣ
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;     // ������Ϣ
        createInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |      // һ����Ϣ
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |   // ��֤�����Ϣ
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;   // ����������Ϣ
        createInfo.pfnUserCallback = debugCallback;  // ���Իص�����
    }

    void setupDebugMessenger();//��Ϊ�Ѵ���messenger������pnext�µĻ�ֻ���ڴ���instanceʱ�Ĵ��󣬵��ǲ�û�д���VkDebugUtilsMessengerEXT��������Ҫ��������
};


