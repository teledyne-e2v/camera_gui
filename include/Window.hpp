#pragma once

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class Window
{
public:
    /**
     * @brief Construct a new Window object
     * 
     */
    Window();
    ~Window();

    /**
     * @brief Get the GLFWWindow object
     * 
     * @return GLFWwindow* 
     */
    GLFWwindow *getWindow();
    /**
     * @brief close the glfw window
     * 
     * @return true 
     * @return false 
     */
    bool shouldClose();

private:
    /**
     * @brief create the glfw window
     * 
     */
    void createGLFWWinfow();
    /**
     * @brief Initialisation of ImGui
     * 
     */
    void initImGui();

private:
    /**
     * @brief glfw window
     * 
     */
    GLFWwindow *window;
};