#pragma once

#include "svl/types.h"

namespace volts::vm
{
    // a defined section of ps3 memory
    struct section
    {
        section(svl::u8* base, svl::u32 len)
            : ptr(base)
            , size(len)
        {}

    private:
        svl::u8* ptr;
        svl::u32 size;
    };

    namespace mem
    {
        // contains all sections
        extern section all;

        // main sections
        extern section main;

        // 64KB user pages
        extern section user64k;

        // 1MB user pages
        extern section user1m;

        // rsx context memory
        extern section rsx;

        // vram
        extern section video;

        // ps3 stack
        extern section stack;

        // spu memory
        extern section spu;
    }

    void init();
}