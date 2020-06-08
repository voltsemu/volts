#pragma once

#include <svl/endian.h>
#include <stdint.h>

namespace elf
{
    template<typename T, svl::ByteOrder Order = svl::ByteOrder::native>
    struct Header
    {
        template<typename V>
        using Val = svl::EndianValue<V, Order>;

        uint32_t magic;

        uint8_t cls;
        uint8_t endian;
        uint8_t version;

        uint8_t abi;
        uint8_t abi_version;

        char pad[7];

        Val<uint16_t> type;
        Val<uint16_t> machine;
        Val<uint32_t> elf_version;

        Val<T> entry;
        Val<T> phdr_offset;
        Val<T> shdr_offset;

        Val<uint32_t> flags;
        Val<uint16_t> size;

        Val<uint16_t> phdr_size;
        Val<uint16_t> phdr_count;

        Val<uint16_t> shdr_size;
        Val<uint16_t> shdr_count;

        Val<uint16_t> strtab;
    };

    static_assert(sizeof(Header<uint32_t>) == 52);
    static_assert(sizeof(Header<uint64_t>) == 64);

    template<typename T, svl::ByteOrder Order = svl::ByteOrder::native>
    struct ProgHeader { };

    template<svl::ByteOrder Order>
    struct ProgHeader<uint32_t, Order>
    {
        template<typename V>
        using Val = svl::EndianValue<V, Order>;

        Val<uint32_t> type;
        Val<uint32_t> offset;

        Val<uint32_t> vaddr;
        Val<uint32_t> paddr;

        Val<uint32_t> size;
        Val<uint32_t> align;
    };

    template<svl::ByteOrder Order>
    struct ProgHeader<uint64_t, Order>
    {
        template<typename V>
        using Val = svl::EndianValue<V, Order>;

        Val<uint32_t> type;
        Val<uint32_t> flags;
        Val<uint64_t> offset;

        Val<uint64_t> vaddr;
        Val<uint64_t> paddr;

        Val<uint64_t> fsize;
        Val<uint64_t> msize;
        Val<uint64_t> align;
    };

    static_assert(sizeof(ProgHeader<uint32_t>) == 24);
    static_assert(sizeof(ProgHeader<uint64_t>) == 56);

    template<typename T, svl::ByteOrder Order = svl::ByteOrder::native>
    struct SectHeader
    {
        template<typename V>
        using Val = svl::EndianValue<V, Order>;

        Val<uint32_t> name;
        Val<uint32_t> type;

        Val<T> flags;
        Val<T> addr;
        Val<T> offset;
        Val<T> size;

        Val<uint32_t> link;
        Val<uint32_t> info;

        Val<T> align;
        Val<T> esize;
    };

    static_assert(sizeof(SectHeader<uint32_t>) == 40);
    static_assert(sizeof(SectHeader<uint64_t>) == 64);
}