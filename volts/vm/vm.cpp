#include "vm.h"

using namespace svl;

namespace volts::vm
{
    u8* base = new svl::byte[256 * 1024 * 1024]();

    svl::u8 read8(addr at)
    {
        return *(u8*)(base + at);
    }

    svl::u16 read16(addr at)
    {
        return *(u16*)(base + at);
    }

    svl::u32 read32(addr at)
    {
        return *(u32*)(base + at);
    }

    svl::u64 read64(addr at)
    {
        return *(u64*)(base + at);
    }
}