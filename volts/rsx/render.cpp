#include "render.h"

#include "backend.h"

#include <spdlog/spdlog.h>

#include <GLFW/glfw3.h>

#include <vector>

#include "imgui/imgui.h"

#include "vfs.h"

#if !SYS_OSX
#   include "ogl/render.h"
#endif

namespace volts::rsx
{
    std::vector<render*>* renders()
    {
        static std::vector<render*> list = {};
        return &list;
    }

    void add(render* r)
    {
        renders()->push_back(r);
    }
    
    GLFWwindow* win;

    GLFWwindow* window()
    {
        return win;
    }

    void run(const std::string& name)
    {
        opengl::connect();

        auto* current = renders()->at(0);

        glfwInit();

        current->preinit();

        win = glfwCreateWindow(640, 480, name.c_str(), nullptr, nullptr);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = vfs::get("imgui.ini").string().c_str();

        ImGui::StyleColorsDark();

        current->postinit();

        while(!glfwWindowShouldClose(win))
        {
            glfwPollEvents();
            current->begin();

            static bool a = true;

            ImGui::ShowDemoWindow(&a);

            ImGui::Begin("aaa");
            ImGui::End();

            ImGui::Render();

            current->end();
        }

        current->cleanup();

        ImGui::DestroyContext();

        glfwDestroyWindow(win);
        glfwTerminate();
    }
}