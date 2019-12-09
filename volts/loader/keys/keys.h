#pragma once

#include <optional>

#include "svl/types.h"

namespace volts::loader
{
    enum class key_type : svl::u32
    {
        lvl0 = 1,
        lvl1 = 2,
        lvl2 = 3,
        app = 4,
        disk_img = 5,
        loader = 6,
        other = 7,
        npdrm = 8,
    };

    struct key
    {
        svl::u64 version;
        svl::u16 revision;
        key_type type;
        svl::u8 erk[32];
        svl::u8 riv[16];
        svl::u8 pub[40];
        svl::u8 priv[21];
        svl::u32 curve;
    };

    namespace keys
    {
        static const svl::u8 free_klic[] = {
            0x72, 0xF9, 0x90, 0x78, 0x8F, 0x9C, 0xFF, 0x74, 0x57, 0x25, 0xF0, 0x8E, 0x4C, 0x12, 0x83, 0x87
        };

        std::optional<key> get_self_key(key_type type, svl::u16 rev, svl::u64 ver);
    }
}