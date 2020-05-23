#include "render.h"
#include <spdlog/spdlog.h>

#include "directx12/dx12.h"
#include "metal/mtl.h"
#include "vulkan/vk.h"


namespace volts::rsx
{
    template<typename T>
    void loop(const char* name, const char* version)
    {
        T render;

        render.init(name, version);

        while(1)
        {
            render.begin();
            render.end();
        }

        render.deinit();
    }

#define ENGINE(name, type) if(strcmp(engine, name) == 0) { loop<type>(name, version); return; }

    void run(const char* engine, const char* name, const char* version)
    {
#if SYS_WINDOWS
        // directx12 is on windows only
        ENGINE("directx12", dx12);
#elif SYS_APPLE
        // apple only supports metal
        ENGINE("metal", mtl);
#endif

        // everything supports vulkan (mac has moltenvk)
        ENGINE("vulkan", vk);

        spdlog::critical("render backend {} not found", engine);
    }
}