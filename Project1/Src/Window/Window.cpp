#include "Window/window.h"

void Window::initWindow(const std::string& title, int width, int height)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(width, height, "Vulkan", NULL, NULL);
}

Window::~Window()
{
    glfwDestroyWindow(window);
}


GLFWwindow* Window::getWindowHandle()
{
    return window;
}
