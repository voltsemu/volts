#pragma once

#include <types.h>

#include <mutex>

namespace volts::vm
{
    using addr = svl::u64;

    void* base(addr of);

    template<typename T>
    T read(addr at)
    {
        return *reinterpret_cast<T*>(base(at));
    }

    template<typename T>
    void write(addr at, T val)
    {
        *reinterpret_cast<T*>(base(at)) = val;
    }

    template<typename T>
    T* ptr(addr at)
    {
        return reinterpret_cast<T*>(base(at));
    }

    template<typename T>
    T& ref(addr at)
    {
        return *reinterpret_cast<T*>(base(at));
    }

    /**
     * @brief link in a free chain
     * 
     */
    struct link
    {
        /// next link in the chain
        link* next;

        /// address of a link
        vm::addr addr;

        /// length of the section
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
            link* end = new link{nullptr, (vm::addr)a + width, 0};
            begin = new link{end, (vm::addr)a, 0};
        }

        ~block();

        /// base address
        void* addr;

        /// width of the memory block
        svl::u64 width;

        /// the default page size
        const svl::u32 page_size;

        /// should pages be offset
        const bool offset_pages;

        /// first link
        link* begin;

        /**
         * @brief allocate pages from the block
         * 
         * @param size size of the block to allocate
         * @param align the alignment of the block
         * @return void* the returned pointer, nullptr if out of memory
         */
        void* alloc(svl::u64 size, svl::u64 align = 0x10000);
        
        /**
         * @brief deallocate a peice of memory in the block
         * 
         * @param ptr the block to deallocate
         */
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