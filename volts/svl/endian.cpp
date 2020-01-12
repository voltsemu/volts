#include "endian.h"
#include "tests.h"
#include "types.h"

namespace svl::endian
{
    std::uint16_t byte_swap(std::uint16_t val)
    {
#if CL_MSVC
        return _byteswap_ushort(val);
#elif CL_CLANG || CL_GNU
        return __builtin_bswap16(val);
#endif
    }

    std::uint32_t byte_swap(std::uint32_t val)
    {
#if CL_MSVC
        return _byteswap_ulong(val);
#elif CL_CLANG || CL_GNU
        return __builtin_bswap32(val);
#endif
    }

    std::uint64_t byte_swap(std::uint64_t val)
    {
#if CL_MSVC
        return _byteswap_uint64(val);
#elif CL_CLANG || CL_GNU
        return __builtin_bswap64(val);
#endif
    }
}

using namespace svl;
using namespace endian;
// 16 bit byte swap test
TEST("bswap16", {
    u16 v1 = 0xFF00;
    u16 ma = 0xFFFF;
    u16 mi = 0x0000;
    u16 r1 = 0x00FF;

    ASSERT(byte_swap(v1) == r1);
    ASSERT(byte_swap(mi) == mi);
    ASSERT(byte_swap(v1) != v1);
    ASSERT(byte_swap(ma) == ma);
    return true;
});

// 32 bit byte swap test
TEST("bswap32", {
    ASSERT(byte_swap(0xFF00FF00u) == 0x00FF00FFu);
    ASSERT(byte_swap(0x00000000u) == 0x00000000u);
    ASSERT(byte_swap(0xFF00FF00u) != 0xFF00FF00u);
    ASSERT(byte_swap(0xFFFFFFFFu) == 0xFFFFFFFFu);
    return true;
});

// 64 bit byte swap test
TEST("bswap64", {
    ASSERT(byte_swap(0xFF00FF00FF00FF00ull) == 0x00FF00FF00FF00FFull);
    ASSERT(byte_swap(0xFF00FF00FF00FF00ull) != 0xFF00FF00FF00FF00ull);
    ASSERT(byte_swap(0x0000000000000000ull) == 0x0000000000000000ull);
    ASSERT(byte_swap(0xFFFFFFFFFFFFFFFFull) == 0xFFFFFFFFFFFFFFFFull);
    return true;
});

// generic byte swap test
TEST("bswapn", {
    auto b16 = byte_swap<u16>(0xFF00);
    auto b32 = byte_swap(0xFF00FF00u);
    auto b64 = byte_swap(0xFF00FF00FF00FF00ull);

    ASSERT(b16 == 0x00FF);

    ASSERT(sizeof(b16) == 2);
    ASSERT(sizeof(b32) == 4);   
    ASSERT(sizeof(b64) == 8);
    return true;
});

TEST("big_endian", {
    big<u16> a = { 0xFF00 };
    ASSERT(a == 0x00FF);
    return true;
});

TEST("little_endian", {
    little<u16> a = { 0xFF00 };
    ASSERT(a == 0xFF00);
    return true;
});

TEST("endian_order", {
    ASSERT(endian_order::native == endian_order::big || endian_order::native == endian_order::little);
    ASSERT(endian_order::big != endian_order::little);
    return true;
});