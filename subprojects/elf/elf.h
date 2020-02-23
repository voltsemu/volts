#pragma once

#include <expected.h>
#include <file.h>
#include <endian.h>
#include <convert.h>
#include <types.h>

namespace elf
{
    namespace cvt = svl::convert;
    
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

        svl::endian::big<svl::u32> vaddress;
        svl::endian::big<svl::u32> paddress;
        
        svl::endian::big<svl::u32> size;
        svl::endian::big<svl::u32> align;
    };

    template<>
    struct program_header<svl::u64>
    {
        svl::endian::big<svl::u32> type;
        svl::endian::big<svl::u32> flags;
        svl::endian::big<svl::u64> offset;

        svl::endian::big<svl::u64> vaddress;
        svl::endian::big<svl::u64> paddress;

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

    /**
     * @brief an elf object
     * 
     * @tparam T elf format, either u32 or u64
     */
    template<typename T>
    struct object
    {
        /// the type of the program headers
        using program_t = program_header<T>;

        /// the type of the section headers
        using section_t = section_header<T>;

        /// the type of the elf header
        using header_t = header<T>;

        /// elf header data
        header_t head;

        /// elf program headers
        std::vector<program_t> progs = {};

        /// elf section headers
        std::vector<section_t> sects = {};

        /// underlying file handle
        svl::file data;

        /**
         * @brief Construct a new elf object
         * 
         * @param d the file to load from
         */
        object(svl::file d)
            : data(d)
        {}
    };

    /**
     * @brief parse an elf from a file stream
     * 
     * @tparam T the elf file type
     * @param stream the stream to parse
     * @return svl::expected<T> if the file was properly formatted then a loaded elf
     *         otherwise nullopt
     */
    template<typename T>
    svl::expected<T> load(svl::file stream)
    {
        T ret = { stream };

        stream.seek(0);

        ret.head = stream.read<typename T::header_t>();

        if(ret.head.magic != cvt::to_u32("\177ELF"))
        {
            return svl::none();
        }

        stream.seek(ret.head.prog_offset);
        ret.progs = stream.read<typename T::program_t>(ret.head.prog_count);

        stream.seek(ret.head.sect_offset);
        ret.sects = stream.read<typename T::section_t>(ret.head.sect_count);

        return ret;
    }

    /// a ppu executable
    using ppu_exec = object<svl::u64>;

    /// a ppu system object
    using ppu_prx = object<svl::u64>;

    /// a spu executable
    using spu_exec = object<svl::u32>;
}
