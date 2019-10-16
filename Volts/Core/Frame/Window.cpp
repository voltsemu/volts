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

        glm::mat4 Matrix;
        glm::vec4 Vec;
        auto T = Matrix * Vec;
    }

    Window::~Window()
    {
        ImGui::DestroyContext();
    }

    void Window::Open()
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        Handle = glfwCreateWindow(720, 480, "Volts", nullptr, nullptr);
    }

    void Window::Close()
    {
        glfwDestroyWindow(Handle);
        glfwTerminate();
    }
}