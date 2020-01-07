#include "vm.h"

using namespace svl;

namespace volts::vm
{
    u8* base = nullptr; 

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
    }
}