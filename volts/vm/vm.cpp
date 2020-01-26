#include "vm.h"

namespace volts::vm
{
    using namespace svl;

    u8* base = nullptr; 

    svl::u8& read8(addr at)
    {
        return *(base + at);
    }

    svl::u16& read16(addr at)
    {
        return *(reinterpret_cast<u16*>(base) + at);
    }

    svl::u32& read32(addr at)
    {
        return *(reinterpret_cast<u32*>(base) + at);
    }

    svl::u64& read64(addr at)
    {
        return *(reinterpret_cast<u64*>(base) + at);
    }

    void* real(addr of)
    {
        return base + of;
    }

    void init()
    {
        delete[] base;

        // todo: make this configurable
        base = new byte[0x100000000ULL];
    }
}