#pragma once

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>
#include<string>
class Window {
public:

public:
    void initWindow(const std::string& title, int width, int height);
    GLFWwindow* getWindowHandle();
private:
    GLFWwindow* window;
};