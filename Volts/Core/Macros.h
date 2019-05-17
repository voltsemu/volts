#pragma once

#include "Meta/Macros.h"

// for concatenating tokens at compile time
// eg CAT(__, AA) expands into __AA
// this also works for macros such as __LINE__ 
// eg CAT(_L_, __LINE) expands into _L_6
#define CAT_INNER(A, B) A##B
#define CAT(A, B) CAT_INNER(A, B)

#if CC_MSVC
#   define PACKED_STRUCT(Name, Fields)  _Pragma pack(push, 1) \
                                        struct Name Fields; \
                                        _Pragma pack(pop)
#else
#   define PACKED_STRUCT(Name, Fields) struct __attribute__((packed)) Name Fields;
#endif