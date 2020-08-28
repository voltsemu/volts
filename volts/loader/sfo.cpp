#include "sfo.h"

#include <svl/cvt.h>

namespace vt::sfo {
    struct header {
        u32 magic;
        little<u32> version;
        little<u32> keys;
        little<u32> data;
        little<u32> len;
    };

    enum struct format : u16 {
        array = 4,
        string = 516,
        integer = 1028
    };

    struct table_entry {
        little<u16> key;
        little<format> fmt;
        little<u32> len;
        little<u32> max;
        little<u32> offset;
    };

    std::optional<data> load(svl::file&& file) {
        auto head = file.read<header>();

        if (head.magic != cvt::u32("\0PSF")) {
            return std::nullopt;
        }

        return {};
    }
}