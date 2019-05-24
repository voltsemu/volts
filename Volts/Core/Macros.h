#pragma once

#include "Meta/Macros.h"

// for concatenating tokens at compile time
// eg CAT(__, AA) expands into __AA
// this also works for macros such as __LINE__ 
// eg CAT(_L_, __LINE) expands into _L_6
#define CAT_INNER(A, B) A##B
#define CAT(A, B) CAT_INNER(A, B)

#if CC_MSVC
// use some funny msvc specific macros to change byte padding to 1 then declare the struct
#   define PACKED_STRUCT(Name, Fields)  __pragma(pack(push, 1)) \
                                        struct Name Fields; \
                                        __pragma(pack(pop))
#elif CC_CLANG
#   define PACKED_STRUCT(Name, Fields) struct __attribute__((packed)) Name Fields;
#elif CC_GCC
#   define PACKED_STRUCT(Name, Fields)  _Pragma pack(1) \
                                        struct Name Fields; \
                                        _Pragma pack()
#endif

#if CC_CLANG
#   define VSCRIPT(...) namespace { __attribute__((constructor)) static void CAT(__, __LINE__)() { __VA_ARGS__ } }
#else
#   define VSCRIPT(...) namespace { static auto CAT(__, __LINE__) = [&] { [&]__VA_ARGS__(); return true; }(); }
#endif

#if OS_WINDOWS
#   include <intrin.h>
namespace Volts
{
    using CPUFlag = Cthulhu::I32;
}
#   define CPUID(I, R) __cpuid(R, I)
#else
#   include <cpuid.h>
namespace Volts
{
    using CPUFlag = Cthulhu::U32;
}
#   define CPUID(I, R) __get_cpuid(I, &R[0], &R[1], &R[2], &R[3])
#endif

#define MMX_FLAG 0x00800000
#define SSE_FLAG 0x02000000
#define SSE2_FLAG 0x04000000
#define SSE3_FLAG 0x00000001
#define SSSE3_FLAG 0x00000200
#define AES_FLAG 0x02000000
#define TSX_FLAG 0x800
#define HLE_FLAG 0x10
#define SHA_FLAG 0x20000000
#define RTM_FLAG 0x00000800
#define TSC_FLAG 0x00000010
#define VAES_FLAG 0x00000200
#define AVX512_FLAG 0x00010000
#define SSE41_FLAG 0x00080000
#define SSE42_FLAG 0x00100000
#define AVX_FLAG 0x10000000
#define AVX2_FLAG 0x00000020
#define SSE4A_FLAG 0x00000040

#if CC_MSVC || CC_INTEL
#   define VINLINE __forceinline
#else
#   define VINLINE __attribute__((always_inline))
#endif

namespace Volts
{
    // signifies padding in a structure
    using Pad = Cthulhu::Byte;
}