#include "vm.h"

#include <cstring>

#include <spdlog/spdlog.h>

namespace volts::vm
{
    using namespace svl;

    block::block(u64 a, u64 w)
        : block((void*)a, w)
    {}

    block::block(void* a, u64 w)
        : addr(base((vm::addr)a))
        , width(w)
    {}

    static u32 align(u64 val, u64 alignment)
    {
        return (val + alignment - 1) & -alignment;
    }

    block::~block()
    {
        
    }

    void* block::alloc(u64 size, u64 alignto)
    {
        return nullptr;
    }

    void block::dealloc(void* ptr)
    {
        
    }

    u8* base_addr = nullptr; 

    void* base(addr of)
    {
        return base_addr + of;
    }

    block* main = nullptr;
    block* user64k = nullptr;
    block* user1m = nullptr;
    block* rsx = nullptr;
    block* video = nullptr;
    block* stack = nullptr;
    block* spu = nullptr;
    block* any = nullptr;

    void init()
    {
        // todo: make this configurable

        // create base memory pointer then zero it out
        base_addr = new byte[0x100000000ULL];
        std::memset(base_addr, 0, 0x100000000ULL);

        {
            main = new block(0x10000, 0x1FFF0000);
            user64k = new block(0x20000000, 0x10000000);

            user1m = nullptr;
            rsx = nullptr;

            video = new block(0xC0000000, 0x10000000);
            stack = new block(0xD0000000, 0x10000000);
            spu = new block(0xE0000000, 0x20000000);

            // map all memory
            any = new block(0x10000, 0xE0000000 + 0x20000000);
        }
    }

    void deinit()
    {
        delete[] base_addr;

        delete main;
        delete user64k;
        
        delete user1m;
        delete rsx;

        delete video;
        delete stack;
        delete spu;
        
        delete any;
    }
}