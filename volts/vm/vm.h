#pragma once

#include "svl/types.h"

namespace volts::vm
{
    using addr = svl::u64;

    void* base(addr of);

    template<typename T>
    T read(addr at)
    {
        return *static_cast<T*>(base(at));
    }

    template<typename T>
    void write(addr at, T val)
    {
        *static_cast<T*>(base(at)) = val;
    }

    template<typename T>
    T* ptr(addr at)
    {
        return static_cast<T*>(base(at));
    }

    template<typename T>
    T& ref(addr at)
    {
        return *static_cast<T*>(base(at));
    }

    enum class block_flags
    {
        large_pages = (1 << 0),
        page_offset = (1 << 1),
    };

    struct block
    {
        block(void* a, svl::u64 w);
        block(svl::u64 a, svl::u64 w);
        void* addr;
        svl::u64 width;

        void* alloc(svl::u64 size, svl::u64 align);
    };

    extern block* main;
    extern block* user64k;
    extern block* user1m;
    extern block* rsx;
    extern block* video;
    extern block* stack;
    extern block* spu;
    extern block* any;

    void init();
    void deinit();
}