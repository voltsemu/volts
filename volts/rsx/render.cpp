#include "render.h"

#include "backend.h"

#include <spdlog/spdlog.h>

#if HAS_VULKAN
#   define GLFW_INCLUDE_VULKAN
#endif

#include <lmw/lmw.h>

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
#   include "ogl/render.cpp"
#endif 

namespace volts::rsx
{
#if 0
    void loop(const char* engine, const char* name, const char* version, render* backend)
    {
        glfwSetErrorCallback([](auto err, auto msg) {
            spdlog::info("glfw error {}: {}", err, msg);
        });

        if(!glfwInit())
        {
            spdlog::critical("failed to initialize glfw");
            return;
        }

        backend->preinit({ name, version });

        GLFWwindow* win = glfwCreateWindow(640, 480, fmt::format("{} {}", name, version).c_str(), nullptr, nullptr);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = vfs::get("imgui.ini").string().c_str();

        ImGui::StyleColorsDark();

        backend->postinit(win);

        while(!glfwWindowShouldClose(win))
        {
            glfwPollEvents();
            
            backend->begin();

            if(static bool a = true; a)
                ImGui::ShowDemoWindow(&a);
            
            backend->end();
        }

        backend->cleanup();

        glfwDestroyWindow(win);
        glfwTerminate();
    }
#endif

    template<typename T>
    void loop(const char* name, const char* version)
    {
        static_assert(std::is_base_of<render, T>::value);

        T backend = T();

        backend.preinit({ name, version });

        lmw::window win(fmt::format("{} {}", name, version).c_str(), 640, 480);

        //IMGUI_CHECKVERSION();
        //ImGui::CreateContext();
        //ImGuiIO& io = ImGui::GetIO();
        //io.IniFilename = vfs::get("imgui.ini").string().c_str();
        
        //ImGui::StyleColorsDark();

        backend.postinit(&win);

        while(win.poll())
            ;
        {
            backend.begin();

            //if(static bool a = true; a)
                //ImGui::ShowDemoWindow(&a);

            backend.end();
        }

        win.close();

        backend.cleanup();
    }

#define ENGINE(name, type) if(strcmp(engine, name) == 0) { loop<type>(name, version); return; }

    void run(const char* engine, const char* name, const char* version)
    {
        //ENGINE("vulkan", vulkan);
#if SYS_OSX
        ENGINE("metal", metal);
#else
        ENGINE("opengl", ogl);

#   if SYS_WINDOWS
        //ENGINE("d3d12", dx12);
#   endif
#endif

        spdlog::critical("render backend {} not found", engine);
        return;   
    }
}