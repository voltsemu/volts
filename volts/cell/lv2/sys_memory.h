#pragma once
#include "svl/types.h"
#include "svl/convert.h"

namespace cell::lv2::memory
{
    enum class page_size : svl::u64
    {
        page_size_1m = 0x400,
        page_size_64k = 0x200,
        mask = 0xF00
    };

    enum class access_rights : svl::u64
    {
        ppu_thread = (1 << 3),
        handler = (1 << 2),
        spu_thread = (1 << 1),
        raw_spu = (1 << 0),

        any = ppu_thread | handler | spu_thread | raw_spu,
        none = 240,

        read_only = (1 << 19),
        read_write = (1 << 18),
    };

    struct info_t
    {

    };

    struct attr_t
    {

    };

    struct container_t
    {

    };

    struct alloca_t
    {

    };
}