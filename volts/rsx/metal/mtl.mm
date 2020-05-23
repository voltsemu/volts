#include "mtl.h"

#include <spdlog/spdlog.h>

namespace volts::rsx
{
    void mtl::init(const char* name, const char* version) 
    { 
        spdlog::info("loading metal backend for {}-{}", name, version);
    }
    
    void mtl::deinit() 
    {

    }

    void mtl::begin() 
    {

    }

    void mtl::end() 
    {

    }
}
