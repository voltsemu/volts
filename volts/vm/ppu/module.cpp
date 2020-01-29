#include "module.h"

#include "vm/vm.h"

#include <xxhash.h>

#include "vfs/vfs.h"

namespace volts::ppu
{
    using namespace svl;
    using namespace loader;

    struct relocation_info
    {
        endian::big<u64> offset;
        pad pad[2];
        u8 idx_val;
        u8 idx_addr;
        endian::big<u32> type;
        endian::big<u64> ptr;
    };

    // TODO: all this
    void load_prx(loader::elf::ppu_prx& mod)
    {
        std::unique_ptr<XXH64_state_t, decltype(&XXH64_freeState)> hasher(XXH64_createState(), XXH64_freeState);

        XXH64_reset(hasher.get(), 0);

        std::vector<elf::program_header<u64>> segments;

        for(auto prog : mod.progs)
        {
            if(prog.type != 1)
                continue;

            if(!prog.file_size)
                continue;

            vm::addr start = vm::round_down(prog.vaddress, vm::page_size);
            u32 round_size = prog.vaddress - start;

            u32 size = vm::page_align(prog.mem_size + round_size);

            mod.data.seek(prog.offset);
            auto sect = mod.data.read<u8>(prog.file_size);

            std::memcpy(vm::real(prog.vaddress), sect.data(), prog.file_size);

            if(prog.mem_size > prog.file_size)
                std::memset(vm::real(prog.vaddress + prog.file_size), 0, prog.mem_size - prog.file_size);

            segments.push_back(prog);
        }

        for(auto sect : mod.sects)
        {

        }

        for(auto prog : mod.progs)
        {
            if(prog.type != 0x700000a4)
                continue;

            mod.data.seek(prog.offset);
            auto bin = mod.data.read<u8>(prog.file_size);
            relocation_info* infos = (relocation_info*)bin.data();

            for(int i = 0; i < prog.file_size; i += sizeof(relocation_info))
            {
                auto rel = *(relocation_info*)(bin.data() + i);

                u32 addr = vm::read32(segments[rel.idx_addr].vaddress + rel.offset);
                u32 data = rel.idx_val == 0xFF ? vm::read64(rel.ptr) : segments[rel.idx_val].vaddress + vm::read64(rel.ptr);

                switch(rel.type)
                {
                case 1:
                    vm::read32(addr) = data;
                    break;
                case 4:
                    vm::read16(addr) = data;
                    break;
                case 5:
                    vm::read16(addr) = data >> 16;
                    break;
                case 6:
                    vm::read16(addr) = (data >> 16) + (data & 0x8000 ? 1 : 0);
                    break;
                case 10:
                    //vm::read
                    break;
                case 11:
                    break;
                case 38:
                    vm::read64(addr) = data;
                    break;
                case 44:
                    vm::read64(addr) = data - addr;
                    break;
                case 57:
                    break;
                default:
                    spdlog::error("invalid relocation type");
                    break;
                }
            }
        }

        /* for(auto& prog : mod.progs)
        {
            XXH64_update(hasher.get(), reinterpret_cast<void*>(&prog.vaddress), sizeof(decltype(prog.vaddress)));
            
            spdlog::info("loading program header");

            if(prog.type == 1)
            {
                if(prog.mem_size)
                {

                }
            }
            else if(prog.type == 0x700000A4)
            {
                continue;
            }
            else
            {
                spdlog::error("invalid segment type");
            }

            mod.data.seek(prog.offset);
            auto data = mod.data.read<svl::byte>(prog.file_size);

            spdlog::info("read in section {} {}", prog.offset, data.size());

            std::memcpy(vm::real(prog.vaddress), data.data(), data.size());
        }

        for(auto& sect : mod.sects)
        {
            XXH64_update(hasher.get(), reinterpret_cast<void*>(&sect.address), sizeof(decltype(sect.address)));

            spdlog::info("loading section header");

            mod.data.seek(sect.offset);
            auto data = mod.data.read<svl::byte>(sect.size);
            std::memcpy(vm::real(sect.address), data.data(), data.size());
        } */

        auto hash = XXH64_digest(hasher.get());
        spdlog::info("prx hash: {}", hash);
    }
}