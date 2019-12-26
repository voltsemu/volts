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

    void init()
    {
        delete[] base;
        // todo: make this configurable
        base = new svl::byte[512 * 1024 * 1024]();

        mem::all = section(base, 512 * 1024 * 1024);
        mem::main = section(base, 256 * 1024 * 1024);
        mem::user64k = section(base, 256 * 1024 * 1024);
        
        // user1m: todo
        // rsx: todo

        mem::video = section(base + (256 * 1024 * 1024), 256 * 1024 * 1024);
        mem::stack = section()
    }
}