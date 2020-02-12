#include "render.h"

#include "backend.h"

#include <spdlog/spdlog.h>

#include <GLFW/glfw3.h>

#include <vector>

#include "imgui/imgui.h"

#include "vfs.h"

#include <platform.h>

#if !SYS_OSX
#   include "ogl/render.h"
#else
#   include "metal/render.h"
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
#if !SYS_OSX
        opengl::connect();
#else
        metal::connect();
#endif


        auto* current = renders()->at(0);

        glfwSetErrorCallback([](auto err, auto msg) {
            spdlog::info("glfw error {}: {}", err, msg);
        });

        if(!glfwInit())
        {
            spdlog::critical("failed to initialize glfw");
            return;
        }

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

            current->end();
        }

        current->cleanup();

        glfwDestroyWindow(win);
        glfwTerminate();
    }
}