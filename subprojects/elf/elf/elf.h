#pragma once

#include <svl/endian.h>

namespace elf
{
    template<typename T>
    struct Header
    {
        uint32_t magic;

        uint8_t cls;
        uint8_t endian;
        uint8_t version;
        
        uint8_t abi;
        uint8_t abi_version;

        char pad[7];

        uint16_t type;
        uint16_t machine;
        uint32_t elf_version;

        T entry;
        T phdr_offset;
        T shdr_offset;

        uint32_t flags;
        uint16_t size;

        uint16_t phdr_size;
        uint16_t phdr_count;

        uint16_t shdr_size;
        uint16_t shdr_count;

        uint16_t strtab;
    };

    template<typename T>
    struct ProgHeader
    {

    };

    template<typename T>
    struct SectHeader
    {

    };
}