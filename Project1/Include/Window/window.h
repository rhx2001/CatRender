#pragma once
#include<string>
#include<GLFW/glfw3.h>
#include <memory>
class Window {
public:

public:
    void initWindow(const std::string& title, int width, int height);
    GLFWwindow* getWindowHandle();
private:
    GLFWwindow* window;
};