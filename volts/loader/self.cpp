#include "volts/loader/self.h"
#include "volts/loader/common.h"

#include <svl/convert.h>
#include <svl/logger.h>

#include <elf/elf.h>

namespace volts::self
{
    namespace cvt = svl::convert;

    // TODO: we should probably be less brutal about errors
    // killing the entire app because we failed to parse a file
    // seems a bit extreme

    svl::File load(svl::File &&source)
    {
        using ElfHeader = elf::Header<uint64_t, svl::ByteOrder::big>;
        using ProgHeader = elf::ProgHeader<uint64_t, svl::ByteOrder::big>;
        using SectHeader = elf::SectHeader<uint64_t, svl::ByteOrder::big>;

        auto sce_header = source.read<sce::Header>();

        if (sce_header.magic != cvt::u32("SCE\0"))
            svl::fatal("invalid SCE magic");

        auto self_header = source.read<self::Header>();
        auto appinfo = source.read<AppInfo>();
        auto ehdr = source.read<ElfHeader>();

        if (ehdr.magic != cvt::u32("\177ELF"))
            svl::fatal("invalid ELF magic");

        auto phdrs = source.read<ProgHeader>(ehdr.phdr_count);

        // TODO: figure out how to properly calculate this offset
        source.seek(source.size() - (ehdr.shdr_count * sizeof(SectHeader)));
        auto shdrs = source.read<SectHeader>(ehdr.shdr_count);

        source.seek(self_header.sect_info_offset);
        auto sects = source.read<metadata::Section>(ehdr.phdr_count);

        svl::info("{} {} {} {}", ehdr.phdr_offset, ehdr.shdr_offset, ehdr.phdr_count, ehdr.shdr_count);
        svl::info("uhhh {} {}", source.size() - (ehdr.shdr_count * sizeof(SectHeader)), ehdr.shdr_offset);

        for (auto prog : phdrs)
        {
            svl::info("prog 0x{:x} {} {}", prog.type, prog.paddr, prog.fsize);
        }

        svl::info("=====");

        for (auto sect : shdrs)
        {
            svl::info("sect {:x} {} {}", sect.type, sect.offset, sect.size);
        }

        for (auto sect : sects)
        {
            svl::info("msect {:x}", sect.type);
        }

        return svl::stream();
    }
}