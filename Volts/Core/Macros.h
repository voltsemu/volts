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