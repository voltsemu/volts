#include "render.h"

#include "backend.h"

#include <spdlog/spdlog.h>

#include <GLFW/glfw3.h>

#include <vector>

#include "imgui/imgui.h"

#include "imfilebrowser.h"

#include "vfs.h"

#include <platform.h>

#if !SYS_OSX
#   include "ogl/render.h"
#   include "vulkan/render.h"
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
        vulkan::connect();
#   if SYS_WINDOWS
        directx12::connect();
#   endif
#else
        metal::connect();
#endif

        // TODO: configure
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

        //ImGui::FileBrowser file_dialog;

        while(!glfwWindowShouldClose(win))
        {
            glfwPollEvents();
            current->begin();

            static bool a = true;

            ImGui::ShowDemoWindow(&a);

            ImGui::Begin("utils");
            /*
            if(ImGui::Button("decrypt pup"))
            {
                file_dialog.SetTitle("pup");
                file_dialog.SetTypeFilters({ ".PUP" });
                file_dialog.Open();
            }

            if(ImGui::Button("decrypt self"))
            {
                file_dialog.SetTitle("self");
                file_dialog.SetTypeFilters({ ".SELF", ".BIN" });
                file_dialog.Open();
            }
            */
            ImGui::End();
            /*
            file_dialog.Display();

            if(file_dialog.HasSelected())
            {
                spdlog::info("selected {}", file_dialog.GetSelected().string());
                file_dialog.ClearSelected();
            }
            */
            current->end();
        }

        current->cleanup();

        glfwDestroyWindow(win);
        glfwTerminate();
    }
}