#include "window.h"

#include <platform.h>

#if SYS_WINDOWS
    // fix glfw redefining APIENTRY
#   include <Windows.h>
#endif

#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

#include <alert.h>

#include <vfs.h>

#include <imgui.h>

#include <imgui/examples/imgui_impl_glfw.h>

namespace volts::window
{
    GLFWwindow* win = nullptr;

    void run()
    {
        glfwSetErrorCallback([](int err, const char* desc) {
            spdlog::error("glfw error: {} {}", err, desc);
        });

        if(!glfwInit())
        {
            alert::alert(L"GLFW error", L"failed to initialize glfw");
            return;
        }

        // we will provide our own graphics api stuff
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        // disable vsync
        glfwWindowHint(GLFW_DOUBLEBUFFER, GL_FALSE);

        win = glfwCreateWindow(640, 480, "volts", nullptr, nullptr);

        if(!win)
        {
            alert::alert(L"GLFW error", L"failed to create window");
            return;
        }

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = vfs::get("imgui.ini").string().c_str();

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(win, true);

        while(!glfwWindowShouldClose(win))
        {
            glfwPollEvents();
        }

        glfwDestroyWindow(win);
        glfwTerminate();
    }

    void set_render(const std::string& name)
    {

    }

    void set_title(const std::wstring& title)
    {

    }
}

/*
#if SYS_WINDOWS
#   include <windows.h>
#endif

#include <GLFW/glfw3.h>

#include "window.h"

#include <spdlog/spdlog.h>

#include <alert/alert.h>

#include <spdlog/sinks/base_sink.h>

#include <examples/imgui_impl_glfw.h>

#include "rsx/rsx.h"

namespace volts::gui
{
    static GLFWwindow* window = nullptr;

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

        while(!glfwWindowShouldClose(window))
        {
            ImGui_ImplGlfw_NewFrame();

            ImGui::NewFrame();
            ImGui::Begin("aaa");
            ImGui::End();

            ImGui::Render();
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
}*/