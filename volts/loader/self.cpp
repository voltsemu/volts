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
        auto sce_header = source.read<sce::Header>();

        if (sce_header.magic != cvt::u32("SCE\0"))
            svl::fatal("invalid SCE magic");

        auto self_header = source.read<self::Header>();
        auto appinfo = source.read<AppInfo>();
        auto ehdr = source.read<elf::Header<uint64_t>>();

        if (ehdr.magic != cvt::u32("\177ELF"))
            svl::fatal("invalid ELF magic");

        source.seek(ehdr.phdr_offset);
        auto phdrs = source.read<elf::ProgHeader<uint64_t>>(ehdr.phdr_count);

        source.seek(ehdr.shdr_offset);
        auto shdrs = source.read<elf::SectHeader<uint64_t>>(ehdr.shdr_count);



        return svl::stream();
    }
}