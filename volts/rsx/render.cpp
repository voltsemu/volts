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
#   if HAS_VULKAN
#       include "vulkan/render.h"
#   endif
#   if SYS_WINDOWS
#       include "dx12/render.h"
#   endif
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
#   if HAS_VULKAN
        vulkan::connect();
#   endif
#   if SYS_WINDOWS
        directx12::connect();
#   endif
#else
        metal::connect();
#endif

        // TODO: configure
        auto* current = renders()->at(1);

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

        double last_frame = 0.0;
        int frames = 0;

        while(!glfwWindowShouldClose(win))
        {
            frames++;
            glfwPollEvents();
            current->begin();

            if(static bool a = true; a)
                ImGui::ShowDemoWindow(&a);


            ImGui::Begin("utils");
            ImGui::Text("fps %.2f", frames/last_frame);
            ImGui::End();
            
            current->end();
            last_frame = glfwGetTime();
        }

        current->cleanup();

        glfwDestroyWindow(win);
        glfwTerminate();
    }
}