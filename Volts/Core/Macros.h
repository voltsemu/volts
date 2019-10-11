#pragma once

#include <Meta/Macros.h>
#include <Meta/Aliases.h>

#include <chrono>

// for concatenating tokens at compile time
// eg CAT(__, AA) expands into __AA
// this also works for macros such as __LINE__
// eg CAT(_L_, __LINE) expands into _L_8
#define CAT_INNER(A, B) A##B
#define CAT(A, B) CAT_INNER(A, B)

#define INNER_STR(...) #__VA_ARGS__
#define STR(...) INNER_STR(__VA_ARGS__)

#if CC_MSVC
// use some funny msvc specific macros to change byte padding to 1 then declare the struct
#   define PACKED_STRUCT(Name, Fields)  __pragma(pack(push, 1)) \
                                        struct Name Fields; \
                                        __pragma(pack(pop))
#elif CC_CLANG
#   define PACKED_STRUCT(Name, Fields) struct __attribute__((packed)) Name Fields;
#elif CC_GCC
#   define PACKED_STRUCT(Name, Fields)  struct Name Fields __attribute__((packed));
#endif

#if CC_CLANG
#   define VSCRIPT(...) namespace { __attribute__((constructor)) static void CAT(__, __LINE__)() { __VA_ARGS__ } }
#else
#   define VSCRIPT(...) namespace { static auto CAT(__, __LINE__) = [&] { [&]__VA_ARGS__(); return true; }(); }
#endif

// when i want something to be inline the compiler better make it inline
#if CC_MSVC || CC_INTEL
#   define VINLINE __forceinline
#else
#   define VINLINE __attribute__((always_inline))
#endif

namespace Volts
{
    // signifies padding in a structure
    using Pad = Cthulhu::Byte;
    using TimePoint = decltype(std::chrono::high_resolution_clock::now());
    using TimeDiff = std::chrono::duration<double, std::milli>;

}
