#include "render.h"

#include "backend.h"

#include <spdlog/spdlog.h>

#if HAS_VULKAN
#   define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>

#include <vector>

#include "imgui/imgui.h"

#include "vfs.h"

#include <svl/platform.h>

#include "vulkan/render.h"

#if SYS_WINDOWS
#   include "dx12/render.h"
#endif

#if SYS_OSX
#   include "metal/render.h"
#else
#   include "ogl/render.h"
#endif 

namespace volts::rsx
{
    std::vector<render*>* renders()
    {
        static std::vector<render*> list = {};
        return &list;
    }

    void add(render* r) { renders()->push_back(r); }
    
    void run(const std::string& render_name, const char* name, const char* version)
    {
        // all platforms have vulkan (or moltenvk)
        vk::connect();

#if SYS_WINDOWS
        directx12::connect();
#endif

#if SYS_OSX
        metal::connect();
#else
        // macos doesnt support opengl anymore so dont use it on mac
        opengl::connect();
#endif

        auto render = std::find_if(renders()->begin(), renders()->end(), [&render_name](auto* render) { return render->name() == render_name; });
        
        if(render == renders()->end())
        {
            spdlog::critical("render backend {} not found", render_name);
            return;
        }

        auto* current = *render;

        glfwSetErrorCallback([](auto err, auto msg) {
            spdlog::info("glfw error {}: {}", err, msg);
        });

        if(!glfwInit())
        {
            spdlog::critical("failed to initialize glfw");
            return;
        }

        current->preinit({ name, version });

        GLFWwindow* win = glfwCreateWindow(640, 480, fmt::format("{} {}", name, version).c_str(), nullptr, nullptr);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = vfs::get("imgui.ini").string().c_str();

        ImGui::StyleColorsDark();

        current->postinit(win);

        while(!glfwWindowShouldClose(win))
        {
            glfwPollEvents();
            
            current->begin();

            if(static bool a = true; a)
                ImGui::ShowDemoWindow(&a);
            
            current->end();
        }

        current->cleanup();

        glfwDestroyWindow(win);
        glfwTerminate();
    }
}