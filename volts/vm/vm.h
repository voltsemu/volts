#pragma once

#include "svl/types.h"

#include <mutex>

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

    struct link
    {
        link* next;
        link* behind;

        vm::addr addr;
        svl::u32 len;
    };

    struct block
    {
        template<typename T>
        block(T a, svl::u64 w, svl::u32 ps = 0x10000, bool op = false)
            : addr(base((vm::addr)a))
            , width(w)
            , page_size(ps)
            , offset_pages(op)
        {
            begin = new link{nullptr, nullptr, (vm::addr)a, 0};
            end = new link{nullptr, begin, (vm::addr)a + width, 0};
            begin->next = end;
        }

        ~block();

        void* addr;
        svl::u64 width;

        const svl::u32 page_size;
        const bool offset_pages;

        link* begin;
        link* end;

        // returns vm address, not real address (although technically it would work either way)
        void* alloc(svl::u64 size, svl::u64 align = 0x10000);
        void dealloc(void* ptr);

    private:
        std::mutex mut;
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