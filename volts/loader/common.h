#pragma once

#include <stdint.h>

#include <svl/endian.h>

namespace volts
{
    using namespace svl;

    namespace sce
    {
        struct Header
        {
            uint32_t magic;
            Big<uint32_t> version;
            Big<uint16_t> type;
            Big<uint16_t> category;
            Big<uint32_t> metadata_start;
            Big<uint64_t> metadata_end;
            Big<uint64_t> size;
        };
    }

    namespace self
    {
        struct Header
        {
            Big<uint64_t> type;
            Big<uint64_t> app_info;
            Big<uint64_t> elf_offset;
            Big<uint64_t> prog_offset;
            Big<uint64_t> sect_offset;
            Big<uint64_t> sect_info_offset;
            Big<uint64_t> version_offset;
            Big<uint64_t> control_offset;
            Big<uint64_t> control_length;
            char pad[8];
        };

        struct ControlFlags
        {
            Big<uint32_t> flags;
            char pad[28];
        };

        struct Digest48
        {
            char constant[20];
            char digest[20];
            Big<uint64_t> min_version;
        };

        struct Digest32
        {
            char digest[20];
            char pad[12];
        };

        struct NPDRMInfo
        {
            uint32_t magic;
            Big<uint32_t> version;
            Big<uint32_t> drm_type;
            Big<uint32_t> app_type;
            char content_id[48];
            char digest[16];
            char inv_digest[16];
            char xor_digest[16];
            char pad[16];
        };

        struct ControlInfo
        {
            uint32_t type;
            uint32_t size;
            uint64_t next;

            union {
                ControlFlags flags;
                Digest32 digest32;
                Digest48 digest48;
                NPDRMInfo npdrm_info;
            };
        };
    }

    namespace metadata
    {
        struct Section
        {
            Big<uint64_t> offset;
            Big<uint64_t> size;
            Big<uint32_t> type;
            Big<uint32_t> index;
            Big<uint32_t> hash_algo;
            Big<uint32_t> hash_index;
            Big<uint32_t> encrypted;

            Big<uint32_t> key_index;
            Big<uint32_t> iv_index;

            Big<uint32_t> compressed;
        };

        struct Header
        {
            Big<uint64_t> length;
            Big<uint32_t> algo;
            Big<uint32_t> sect_count;
            Big<uint32_t> key_count;
            Big<uint32_t> size;
            char pad[8];
        };

        struct Info
        {
            char key[16];
            char key_pad[16];

            char iv[16];
            char iv_pad[16];
        };
    }

    struct AppInfo
    {
        Big<uint64_t> auth;
        Big<uint32_t> vendor;
        Big<uint32_t> type;
        Big<uint64_t> version;
        char pad[8];
    };

    static_assert(sizeof(sce::Header) == 32);

    static_assert(sizeof(self::Header) == 80);
    static_assert(sizeof(self::ControlFlags) == 32);
    static_assert(sizeof(self::Digest48) == 48);
    static_assert(sizeof(self::Digest32) == 32);
    static_assert(sizeof(self::NPDRMInfo) == 128);

    static_assert(sizeof(metadata::Section) == 48);
    static_assert(sizeof(metadata::Header) == 32);
    static_assert(sizeof(metadata::Info) == 64);

    static_assert(sizeof(AppInfo) == 32);
}