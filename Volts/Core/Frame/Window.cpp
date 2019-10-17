#include "Window.h"

#include "imgui.h"

#include <stdio.h>

namespace Volts
{
    static void error_callback(int error, const char* description)
    {
        puts(description);
    }

    Window::Window()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        glfwSetErrorCallback(error_callback);

        if(!glfwInit())
            return;
    }

    Window::~Window()
    {
        ImGui::DestroyContext();
    }

    void Window::Open()
    {
        // we might not use opengl
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        // disable vsync
        glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);
        
        Handle = glfwCreateWindow(720, 480, "Volts", nullptr, nullptr);
    }

    void Window::Close()
    {
        glfwDestroyWindow(Handle);
        glfwTerminate();
    }
}