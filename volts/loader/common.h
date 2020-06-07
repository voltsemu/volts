#pragma once

#include <stdint.h>

#include <svl/endian.h>

namespace volts
{
    using namespace svl;

    namespace sce
    {
        struct Header
        {
            uint32_t magic;
            Big<uint32_t> version;
            Big<uint16_t> type;
            Big<uint16_t> category;
            Big<uint32_t> metadata_start;
            Big<uint64_t> metadata_end;
            Big<uint64_t> size;
        };
    }

    namespace self
    {
        struct Header
        {
            Big<uint64_t> type;
            Big<uint64_t> app_info;
            Big<uint64_t> elf_offset;
            Big<uint64_t> prog_offset;
            Big<uint64_t> sect_offset;
            Big<uint64_t> sect_info_offset;
            Big<uint64_t> version_offset;
            Big<uint64_t> control_offset;
            Big<uint64_t> control_length;
            char pad[8];
        };
    }

    struct AppInfo
    {
        Big<uint64_t> auth;
        Big<uint32_t> vendor;
        Big<uint32_t> type;
        Big<uint64_t> version;
        char pad[8];
    };

    static_assert(sizeof(sce::Header) == 32);
    static_assert(sizeof(self::Header) == 80);
    static_assert(sizeof(AppInfo) == 32);
}