#include "render.h"
#include <spdlog/spdlog.h>


namespace volts::rsx
{
    template<typename T>
    void loop(const char* name, const char* version)
    {

    }

#define ENGINE(name, type) if(strcmp(engine, name) == 0) { loop<type>(name, version); return; }

    void run(const char* engine, const char* name, const char* version)
    {
        spdlog::critical("render backend {} not found", engine);
        return;
    }
}