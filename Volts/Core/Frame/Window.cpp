#include "Window.h"

#include "imgui.h"

#include <stdio.h>

#include "Core/Emulator.h"

namespace Volts
{
    static void ErrCallback(int Err, const char* Desc)
    {
        VERROR("GLFW Error {} {}", Err, Desc);
    }

    static void ResizeCallback(GLFWwindow* Window, int NewWidth, int NewHeight)
    {
        VTRACE("Resizing window to {}x{}", NewWidth, NewHeight);
        Emulator::Get()
            ->Render.Current()
            ->Resize((U32)NewWidth, (U32)NewHeight);
    }

    Window::Window()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        glfwSetErrorCallback(ErrCallback);

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

        glfwSetFramebufferSizeCallback(Handle, ResizeCallback);
    }

    void Window::Close()
    {
        glfwDestroyWindow(Handle);
        glfwTerminate();
    }
}