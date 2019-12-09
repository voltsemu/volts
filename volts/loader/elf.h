#pragma once

#include "svl/endian.h"
#include "svl/convert.h"
#include "svl/types.h"

namespace volts::loader::elf
{
    enum class type : svl::u16
    {
        none = 0,
        rel = 1,
        exec = 2,
        dyn = 3,
        core = 4,

        prx = 65444,

        psv1 = 65024,
        psv2 = 65028,
    };

    enum class machine : svl::u16
    {
        ppc64 = 21,
        spu = 23,
    };

    enum class os : svl::u8
    {
        none = 0,
        lv2 = 102,
    };

    template<typename T>
    struct header
    {
        svl::u32 magic;

        svl::u8 cls;
        svl::u8 endian;
        svl::u8 elf_version1;
        
        os abi;
        svl::u8 abi_verson;

        svl::pad padding[7];

        svl::endian::big<type> elf_type;
        svl::endian::big<machine> machine_type;
        svl::endian::big<svl::u32> elf_version2;

        svl::endian::big<T> entry;
        svl::endian::big<T> prog_offset;
        svl::endian::big<T> sect_offset;

        svl::endian::big<svl::u32> flags;
        svl::endian::big<svl::u16> header_size;

        svl::endian::big<svl::u16> prog_entry_size;
        svl::endian::big<svl::u16> prog_count;

        svl::endian::big<svl::u16> sect_entry_size;
        svl::endian::big<svl::u16> sect_count;

        svl::endian::big<svl::u16> string_index;
    };

    static_assert(sizeof(header<svl::u32>) == 52);
    static_assert(sizeof(header<svl::u64>) == 64);

    template<typename T>
    struct program_header {};

    template<>
    struct program_header<svl::u32>
    {
        svl::endian::big<svl::u32> type;
        svl::endian::big<svl::u32> offset;

        svl::endian::big<svl::u32> vaddres;
        svl::endian::big<svl::u32> paddres;
        
        svl::endian::big<svl::u32> size;
        svl::endian::big<svl::u32> align;
    };

    template<>
    struct program_header<svl::u64>
    {
        svl::endian::big<svl::u32> type;
        svl::endian::big<svl::u64> flags;
        svl::endian::big<svl::u64> offset;

        svl::endian::big<svl::u64> vaadres;
        svl::endian::big<svl::u64> paddres;

        svl::endian::big<svl::u64> file_size;
        svl::endian::big<svl::u64> mem_size;
        svl::endian::big<svl::u64> align;
    };

    template<typename T>
    struct section_header
    {
        svl::endian::big<svl::u32> name;
        svl::endian::big<svl::u32> type;

        svl::endian::big<T> flags;
        svl::endian::big<T> address;
        svl::endian::big<T> offset;
        svl::endian::big<T> size;

        svl::endian::big<svl::u32> link;
        svl::endian::big<svl::u32> info;

        svl::endian::big<T> align;
        svl::endian::big<T> entry_size;
    };
}