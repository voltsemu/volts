#include "vm.h"

using namespace svl;

namespace volts::vm
{
    u8* base = nullptr; 

    namespace mem
    {
        section all;
        section main;
        section user64k;
        section user1m;
        section rsx;
        section video;
        section stack;
        section spu;
    }    

    svl::u8& read8(addr at)
    {
        return (base + at)[0];
    }

    svl::u16& read16(addr at)
    {
        return (reinterpret_cast<u16*>(base) + at)[0];
    }

    svl::u32& read32(addr at)
    {
        return (reinterpret_cast<u32*>(base) + at)[0];
    }

    svl::u64& read64(addr at)
    {
        return (reinterpret_cast<u64*>(base) + at)[0];
    }

    void init()
    {
        delete[] base;

        // todo: make this configurable
        base = new byte[1024 * 1024 * 1024 * 4]();

        mem::all = section(base, 1024 * 1024 * 1024 * 4);
        mem::main = section(base, 256 * 1024 * 1024);
        mem::user64k = section(base, 256 * 1024 * 1024);
        
        // user1m: todo
        // rsx: todo

        mem::video = section(base + (256 * 1024 * 1024), 256 * 1024 * 1024);
        //mem::stack = section();
    }
}