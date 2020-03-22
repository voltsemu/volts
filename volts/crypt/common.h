#pragma once

#include <endian.h>
#include <convert.h>

#include <zlib.h>

#include <spdlog/spdlog.h>

#include <aes.h>

#include "keys/keys.h"


namespace volts::crypt
{
    using svl::endian::big;

    using namespace svl;
    namespace cvt = svl::convert;

    namespace sce
    {
        struct header
        {
            u32 magic;
            big<u32> version;
            big<u16> type;
            big<u16> category;
            big<u32> metadata_start;
            big<u64> metadata_end;
            big<u64> size;
        };

        static_assert(sizeof(header) == 32);
    }

    namespace self
    {
        struct header
        {
            big<u64> type;
            big<u64> app_info_offset;
            big<u64> elf_offset;
            big<u64> prog_offset;
            big<u64> sect_offset;
            big<u64> sect_info_offset;
            big<u64> version_offset;
            big<u64> control_offset;
            big<u64> control_length;
            padding<8> pad;  
        };

        static_assert(sizeof(header) == 80);

        struct control_info
        {
            u32 type;
            u64 size;
            u64 next;

            union
            {
                struct 
                {
                    big<u32> flags;
                    padding<28> pad;
                } control_flags;

                static_assert(sizeof(control_flags) == 32);

                struct 
                {
                    u8 constant[20];
                    u8 elf_digest[20];
                    big<u64> required_version;
                } elf_digest_48;

                static_assert(sizeof(elf_digest_48) == 48);

                struct
                {
                    u8 const_or_digest[20];
                    padding<12> pad;
                } elf_digest_32;

                static_assert(sizeof(elf_digest_32) == 32);

                struct 
                {
                    u32 magic;
                    big<u32> version;
                    big<u32> drm_type;
                    big<u32> app_type;
                    byte content_id[48];
                    byte digest[16];
                    byte inv_digest[16];
                    byte xor_digest[16];
                    padding<16> pad;
                } npdrm_info;

                static_assert(sizeof(npdrm_info) == 128);
            };
        };
    }

    namespace metadata
    {
        struct info
        {
            byte key[16];
            padding<16> key_pad;

            byte iv[16];
            padding<16> iv_pad;
        };

        static_assert(sizeof(info) == 64);

        struct header
        {
            big<u64> signature_length;
            big<u32> algorithm;
            big<u32> sect_count;
            big<u32> key_count;
            big<u32> header_size;
            padding<8> pad;
        };

        static_assert(sizeof(header) == 32);

        struct section
        {
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

        static_assert(sizeof(section) == 48);
    }

    struct app_info
    {
        big<u64> auth;
        big<u32> vendor;
        big<u32> type;
        big<u64> version;
        padding<8> pad;
    };

    static_assert(sizeof(app_info) == 32);
}