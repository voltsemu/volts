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
                big<u32> flags;

                u8 pad[28];
            };

            struct digest48 {
                u8 constant[20];
                u8 digest[20];
                big<u64> version;
            };

            struct digest32 {
                union {
                    u8 constant[20];
                    u8 digest[20];
                };

                u8 pad[12];
            };

            struct info {
                u32 magic;
                big<u32> version;
                big<u32> drm_type;
                big<u32> app_type;
                u8 id[48];
                u8 digest[16];
                u8 inv[16];
                u8 xor[16];

                u8 pad[16];
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
            u8 key[16];
            u8 key_pad[16];
            
            u8 iv[16];
            u8 iv_pad[16];
        };

        struct header {
            big<u64> signature;
            big<u32> algorithm;
            big<u32> sects;
            big<u32> keys;
            big<u32> size;

            u8 pad[8];
        };

        struct section {
            big<u64> offset;
            big<u64> size;
            big<u32> type;
            big<u32> index;
            big<u32> hash_algo;
            big<u32> hash_index;
            big<u32> encrypted;

            big<u32> key_index;
            big<u32> iv_index;

            big<u32> compressed;
        };
    }

    struct app_info {
        big<u64> auth;
        big<u32> vendor;
        big<u32> type;
        big<u64> version;

        u8 pad[8];
    };
}