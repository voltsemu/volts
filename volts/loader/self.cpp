#include "volts/loader/self.h"
#include "volts/loader/common.h"
#include "volts/loader/keys/keys.h"

#include <svl/convert.h>
#include <svl/logger.h>
#include <svl/macros.h>

#include <crypt/aes.h>

#include <elf/elf.h>

namespace volts::self
{
    namespace cvt = svl::convert;

    // TODO: we should probably be less brutal about errors
    // killing the entire app because we failed to parse a file
    // seems a bit extreme

    // read a single control info entry from a file
    // a control info has variable size so it makes sense to
    // do it in an extra function to keep everything tidy
    self::ControlInfo read_control(svl::File *source)
    {
        self::ControlInfo info;
        info.type = svl::bswap(source->read<uint32_t>());
        info.size = svl::bswap(source->read<uint32_t>());
        info.next = svl::bswap(source->read<uint64_t>());

        if (info.type == 1)
        {
            info.flags = source->read<self::ControlFlags>();
        }
        else if (info.type == 2)
        {
            if (info.size == 48)
            {
                info.digest48 = source->read<self::Digest48>();
            }
            else if (info.size == 32)
            {
                info.digest32 = source->read<self::Digest32>();
            }
        }
        else if (info.type == 3)
        {
            info.npdrm_info = source->read<self::NPDRMInfo>();
        }
        else
        {
            svl::fatal("invalid ControlInfo type {}", info.type);
        }

        return info;
    }

    void decrypt_npdrm(
        const std::vector<self::ControlInfo>& controls,
        const std::vector<uint8_t> meta_key
    )
    {
        (void)meta_key;
        auto ctrl = std::find_if(controls.begin(), controls.end(), [](auto& val) { return val.type == 3; });

        if (ctrl == controls.end())
            return;

        switch (ctrl->npdrm_info.version)
        {
        case 3:
            break;
        default:
            svl::fatal("unsupported license type");
        }
    }

    svl::File load(svl::File &&source, const std::vector<uint8_t>& npdrm_key)
    {
        // self files contain 64 bit big endian elf data
        // we alias that here
        using ElfHeader = elf::Header<uint64_t, svl::ByteOrder::big>;
        using ProgHeader = elf::ProgHeader<uint64_t, svl::ByteOrder::big>;
        using SectHeader = elf::SectHeader<uint64_t, svl::ByteOrder::big>;

        // self files start with an sce header
        auto sce_header = source.read<sce::Header>();

        if (sce_header.magic != cvt::u32("SCE\0"))
            svl::fatal("invalid SCE magic");

        // then comes some more self specific data
        auto self_header = source.read<self::Header>();
        auto appinfo = source.read<AppInfo>();

        // finally we reach the actual elf header
        auto ehdr = source.read<ElfHeader>();

        if (ehdr.magic != cvt::u32("\177ELF"))
            svl::fatal("invalid ELF magic");

        // the program headers are in practice always directly after the elf header
        // if i can find a self where this isnt the case i will update this
        auto phdrs = source.read<ProgHeader>(ehdr.phdr_count);

        // so section headers *should* be where the elf header says they are
        // after some fiddling.
        // problem is that they arent in any of my test data.
        // they are however always at the end of the file so we just
        // read based off of that instead
        // TODO: figure out how to properly calculate this offset
        source.seek(source.size() - (ehdr.shdr_count * sizeof(SectHeader)));
        auto shdrs = source.read<SectHeader>(ehdr.shdr_count);

        // read in control info
        source.seek(self_header.control_offset);
        uint_fast64_t c = 0;
        std::vector<self::ControlInfo> controls;
        while (c < self_header.control_length)
        {
            auto ctrl = read_control(&source);
            c += ctrl.size;

            controls.push_back(ctrl);
        }


        // time to load the rest of the metadata
        source.seek(sce_header.metadata_start + sizeof(sce::Header));

        // read in the metadata info header and sections
        auto meta_info = source.read<metadata::Info>();

        source.seek(self_header.sect_info_offset);
        auto sects = source.read<metadata::Section>(ehdr.phdr_count);




        // decrypt the metadata sections
        usize header_size = sce_header.metadata_end
            - sizeof(sce::Header)
            - sce_header.metadata_start
            - sizeof(metadata::Info);

        source.seek(sce_header.metadata_start + sizeof(sce::Header) + sizeof(metadata::Info));

        // find a decryption key for the file
        auto key = keys::get(
            appinfo.type.as<keys::key_type>(),
            sce_header.type,
            appinfo.version
        );

        auto* headers = (uint8_t*)ALLOCA(header_size);
        source.read(headers, header_size);

        if ((sce_header.type & 0x8000) != 0x8000)
        {
            // if we've got a release SELF and not a debug one we need to decrypt the meta info
            decrypt_npdrm(controls, npdrm_key);

            crypt::aes::Context<256>::dec(key.erk)
                .crypt_cbc<crypt::Mode::dec>(
                    sizeof(metadata::Info),
                    key.riv,
                    (uint8_t*)&meta_info,
                    (uint8_t*)&meta_info
                );
        }

        // the padding should be 0 if decryption was successful
        for (int i = 0; i < 16; i++)
            ENSURE(meta_info.key_pad[i] | meta_info.iv_pad[i]);

        // now we need to decrypt the rest of the headers
        size_t offset = 0;
        uint8_t stream[16] = {};

        crypt::aes::Context<128>::enc(meta_info.key)
            .crypt_ctr(header_size, &offset, meta_info.iv, stream, headers, headers);

        auto meta_head = *(metadata::Header*)headers;

        uint64_t data_len = 0;

        std::vector<metadata::Section> meta_sections;

        // then take the decrypted sections
        for (uint32_t i = 0; i < meta_head.sect_count; i++)
        {
            auto sect = *(metadata::Section*)(headers + sizeof(metadata::Header) + sizeof(metadata::Section) * i);

            if (sect.encrypted == 3)
                data_len += sect.size;

            meta_sections.push_back(sect);
        }

        auto data_keys = headers + sizeof(metadata::Header) + meta_head.sect_count * sizeof(metadata::Section);




        svl::info("size {}", header_size);

        for (auto ctrl : controls)
        {
            svl::info("ctrl {} {} {}", ctrl.type, ctrl.size, ctrl.next);
        }

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
            svl::info("msect {:x} {}", sect.type, sect.size);
        }

        return svl::stream();
    }
}