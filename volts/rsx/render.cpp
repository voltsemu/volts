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
#   include "ogl/render.cpp"
#endif 

namespace volts::rsx
{
    template<typename T>
    void loop(const char* name, const char* version)
    {

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