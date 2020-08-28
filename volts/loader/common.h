#pragma once

#include <svl/types.h>

namespace vt {
    using namespace svl;

    namespace sce {
        struct header {
            u32 magic;
            big<u32> version;
            big<u16> type;
            big<u16> category;
            big<u32> start;
            big<u32> end;
            big<u64> size;
        };
    }

    namespace self {
        struct header {
            big<u64> type;
            big<u64> app_info_offset;
        };

        struct control {
            struct flags {

            };

            struct digest48 {

            };

            struct digest32 {

            };

            struct info {

            };

            u32 type;
            u64 size;
            u64 next;

            union {
                flags flags;
                digest32 digest32;
                digest48 digest48;
                info npdrm;
            };
        };
    }

    namespace metadata {
        struct info {

        };

        struct header {

        };

        struct section {

        };
    }

    struct app_info {

    };
}