#include "vm.h"

#include <cstring>

#include <spdlog/spdlog.h>

namespace volts::vm
{
    using namespace svl;

    u8* base_addr = nullptr; 

#define LOCKED(...) { std::lock_guard<std::mutex> guard(this->mut); { __VA_ARGS__ } }

    static u32 align(u64 val, i64 alignment)
    {
        return (val + alignment - 1) & -alignment;
    }

    static void free_link_chain(link* begin)
    {
        if(begin)
        {
            free_link_chain(begin->next);
            delete begin;
        }
    }

    block::~block()
    {
        free_link_chain(begin);
    }

    vm::addr block::alloc(u64 size, u64 alignto)
    {
        u32 s = align(size, page_size) + (offset_pages ? 0x2000 : 0);

        LOCKED({
            link* cur = begin;
            for(;;)
            {
                // check if the chain is empty
                if(!cur->next)
                {
                    return 0;
                }
                else if(cur->len + cur->addr + s > cur->next->addr)
                {
                    // if there isnt space then check the next link
                    cur = cur->next;
                }
                else
                {
                    // there is space
                    link* in = new link{cur->next, cur->addr + cur->len, s};
                    cur->next = in;
                    return in->addr;
                }
            }
        });
    }

    vm::addr block::falloc(vm::addr addr, u64 size)
    {
        // TODO
        return addr;
    }

    void block::dealloc(vm::addr ptr)
    {
        LOCKED({
            // TODO
        });
    }

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
        spdlog::info("initializing vm memory");
        
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