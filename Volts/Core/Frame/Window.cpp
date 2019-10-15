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

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        Handle = glfwCreateWindow(720, 480, "Volts", nullptr, nullptr);
    }

    Window::~Window()
    {
        ImGui::DestroyContext();
    }

    void Window::Open()
    {

    }

    void Window::Close()
    {
        glfwDestroyWindow(Handle);
        glfwTerminate();
    }
}