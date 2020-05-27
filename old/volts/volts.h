#pragma once

#include <svl/wrapfs.h>

#define V_ASSERT(expr) { if(!(expr)) { spdlog::critical("assert failed {}", #expr); std::abort(); } }

namespace volts
{
    struct config
    {
        bool aes = false;
        fs::path vfs = fs::current_path();
        std::string_view render = "vulkan";
    };
}
