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

#include <platform.h>

#if HAS_VULKAN
#   include "vulkan/render.h"
#endif

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

    void add(render* r)
    {
        renders()->push_back(r);
    }
    
    GLFWwindow* win;

    GLFWwindow* window()
    {
        return win;
    }

    void run(const std::string& name, bool debug)
    {

#if HAS_VULKAN
        vulkan::connect();
#endif

#if SYS_WINDOWS
        directx12::connect();
#endif

#if SYS_OSX
        metal::connect();
#else
        opengl::connect();
#endif

        // TODO: fallback
        auto render = std::find_if(renders()->begin(), renders()->end(), [name](auto* render) { return render->name() == name; });
        
        if(render == renders()->end())
        {
            spdlog::error("render backend {} not found", name);
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

        current->preinit(debug);

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
            
            current->end();
            last_frame = glfwGetTime();
        }

        current->cleanup();

        glfwDestroyWindow(win);
        glfwTerminate();
    }
}