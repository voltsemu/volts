#include <platform.h>

#if SYS_WINDOWS
#   include <windows.h>
#endif

#include <GLFW/glfw3.h>

#include "window.h"

#include <spdlog/spdlog.h>

#include <external/alert/alert.h>

#include <spdlog/sinks/base_sink.h>

#include <examples/imgui_impl_glfw.h>

#include "rsx/rsx.h"

namespace volts::gui
{
    static GLFWwindow* window = nullptr;

    ImGuiLog log;

    void init()
    {
        glfwSetErrorCallback([](int err, const char* desc) {
            spdlog::error("glfw error: {} {}", err, desc);
        });

        if(!glfwInit())
        {
            alert::alert(L"GLFW error", L"Failed to initialize glfw");
        }

        // we will provide our own graphics api stuff
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        // disable vsync
        glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);

        window = glfwCreateWindow(640, 480, "Volts", nullptr, nullptr);

        if(!window)
        {
            alert::alert(L"GLFW error", L"Failed to initialize glfw window");
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGui::StyleColorsDark();

        // we're not using opengl but imgui actually doesnt care what we use
        ImGui_ImplGlfw_InitForOpenGL(window, true);

        rsx::renders()[0]->attach();

        while(!glfwWindowShouldClose(window))
        {
            rsx::renders()[0]->begin();
            ImGui_ImplGlfw_NewFrame();

            ImGui::NewFrame();
            ImGui::Begin("aaa");
            ImGui::End();

            ImGui::Render();

            rsx::renders()[0]->end();
            glfwPollEvents();
        }
    }

    void deinit()
    {
        rsx::renders()[0]->detach();
        ImGui_ImplGlfw_Shutdown();
        
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void set_title(const std::string& str)
    {
        glfwSetWindowTitle(window, str.c_str());
    }

    GLFWwindow* handle() { return window; }
}