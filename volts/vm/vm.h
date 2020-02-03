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

    struct block
    {
        template<typename T>
        block(T a, svl::u64 w, svl::u32 ps = 0x10000, bool op = false)
            : addr(base((vm::addr)a))
            , width(w)
            , page_size(ps)
            , offset_pages(op)
        {}

        ~block();

        void* addr;
        svl::u64 width;

        const svl::u32 page_size;
        const bool offset_pages;

        void* alloc(svl::u64 size, svl::u64 align);
        void dealloc(void* ptr);
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