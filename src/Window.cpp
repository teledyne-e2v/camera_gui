#include "Window.hpp"

#include <cstdio>
#include <iostream>

static void glfw_error_callback(int error, const char *description)
{
    printf("Glfw Error %d: %s\n", error, description);
}

Window::Window()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        throw std::runtime_error("Error: Unable to initialize GLFW");
    }

    createGLFWWinfow();

    if (gl3wInit() != 0)
    {
        throw std::runtime_error("Error: Unable to initialize OpenGL loader");
    }

    initImGui();
}

Window::~Window()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::createGLFWWinfow()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(
        1920, 1080,
        "Camera Controler",
        NULL, NULL);

    if (window == NULL)
    {
        throw std::runtime_error("Error: Unable to create window");
    }

    glfwSetWindowSizeLimits(window, 500, 250, GLFW_DONT_CARE, GLFW_DONT_CARE);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
}
void Window::initImGui()
{
    const char *glsl_version = "#version 130";

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

GLFWwindow *Window::getWindow()
{
    return window;
}

bool Window::shouldClose()
{
    return glfwWindowShouldClose(window);
}