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

        static_assert(sizeof(Header) == 32);
    }
}