#include "endian.h"
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
