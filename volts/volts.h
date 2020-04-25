#pragma once

#include <svl/wrapfs.h>

#define V_ASSERT(expr) { if(!(expr)) { spdlog::critical("assert failed {}", #expr); std::abort(); } }

namespace volts
{
    struct config
    {
        bool aes;
        fs::path vfs;
        std::string_view render;
    };
}
