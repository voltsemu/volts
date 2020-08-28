#pragma once

#include "types.h"

namespace svl::cvt {
    template<auto N>
    constexpr svl::u32 u32(const char(&str)[N]) {
        static_assert(N >= 4);
        return *(svl::u32*)str;
    }
}
