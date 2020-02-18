#include "module.h"

#include "vm.h"

#include <xxhash.h>

#include "vfs.h"

#include <spdlog/spdlog.h>

#include <map>

#include <bitrange.h>

namespace volts::ppu
{
    using namespace svl;
    using namespace endian;

    struct relocation_info
    {
        big<u64> offset;
        big<u16> pad;
        u8 idx_val;
        u8 idx_addr;
        big<u32> type;
        big<u64> ptr;
    };

    static_assert(sizeof(relocation_info) == 24);

    struct segment
    {
        vm::addr addr;
        u64 size;
        u32 type;
        u32 flags;
        u64 file_size;
    };

    std::map<u32, u32> load_symbols(u32 front, u32 back)
    {
        struct module_info
        {
            u8 len;
            pad unk1;

            big<u16> version;
            big<u16> attrib;
            big<u16> funcs;
            big<u16> vars;
            big<u16> tlsvars;
            
            u8 hash;
            u8 tlshash;
            pad unk2[2];
            
            big<vm::addr> name;
            big<vm::addr> nids;
            big<vm::addr> addrs;
            big<vm::addr> vnids;
            big<vm::addr> vstubs;
            
            pad unk3[8];
        };

        std::map<u32, u32> symbols = {};

        auto addr = front;

        while(addr < back)
        {
            spdlog::info("offset {}", addr);
            auto lib = vm::read<module_info>(addr);

            spdlog::info("lib(len={},version={},attrib={},funcs={},vars={},tlsvars={},hash={},tlshash={},name={},nids={},addrs={},vnids={},vstubs={})", 
                lib.len,
                lib.version.get(),
                lib.attrib.get(),
                lib.funcs.get(),
                lib.vars.get(),
                lib.tlsvars.get(),
                lib.hash,
                lib.tlshash,
                lib.name.get(),
                lib.nids.get(),
                lib.addrs.get(),
                lib.vnids.get(),
                lib.vstubs.get()
            );

            spdlog::info("name offset {}", lib.name.get());

            // special symbols
            if(lib.name.get() == 0)
            {
                auto* nids = (big<vm::addr>*)vm::base(lib.nids);
                auto* addrs = (big<vm::addr>*)vm::base(lib.addrs);

                for(int i = 0, end = lib.funcs + lib.vars; i < end; i++)
                {
                    u32 nid = nids[i];
                    u32 addr = addrs[i];

                    spdlog::info("{} = {}", nid, addr);
                }
                // we add 44 because thats how large module_info is meant to be but msvc
                // refuses to pack the thing correctly
                addr += lib.len ? lib.len : 44;
                continue;
            }

            std::string name = (char*)vm::base(lib.name.get());
            spdlog::info("symbol name: {}", name);

            addr += lib.len ? lib.len : 44;
        }

        return symbols;
    }

    // TODO: all this
    void load_prx(elf::ppu_prx& mod)
    {
        std::unique_ptr<XXH64_state_t, decltype(&XXH64_freeState)> hasher(XXH64_createState(), XXH64_freeState);
        XXH64_reset(hasher.get(), 0);
        
        std::vector<segment> segments;

        // load data into memory
        for(auto prog : mod.progs)
        {
            if(prog.type != 1 || !prog.file_size)
                continue;

            mod.data.seek(prog.offset);
            auto dat = mod.data.read<u8>(prog.file_size);

            vm::addr addr = (vm::addr)vm::main->alloc(prog.mem_size);

            if(!addr)
                spdlog::error("out of memory");

            spdlog::info("loaded module at {}", addr);
            
            std::memcpy(vm::base(addr), dat.data(), prog.file_size);
        
            segments.push_back(segment{
                addr,
                prog.mem_size,
                prog.type,
                prog.flags,
                prog.file_size
            });

            XXH64_update(hasher.get(), &prog.vaddress, sizeof(prog.vaddress));
        }

        std::vector<segment> sections;

        // get information about relocations
        for(auto sect : mod.sects)
        {
            if(sect.type != 1)
                continue;

            for(int i = 0; i < segments.size(); i++)
            {
                u32 sect_addr = mod.progs[i].vaddress;
                if(sect.address >= sect_addr && sect.address < sect_addr + mod.progs[i].mem_size)
                {
                    sections.push_back(segment{
                        sect.address - sect_addr + segments[i].addr,
                        sect.size,
                        sect.type,
                        sect.flags & 7,
                        0
                    });
                    break;
                }
            }
        }

        // do the relocations
        for(auto prog : mod.progs)
        {
            if(prog.type != 0x700000A4)
                continue;

            mod.data.seek(prog.offset);
            auto relocs = mod.data.read<relocation_info>(prog.file_size / sizeof(relocation_info));

            for(auto reloc : relocs)
            {
                auto addr = vm::read<u32>(segments.at(reloc.idx_addr).addr + reloc.offset);
                auto data = reloc.idx_val == 0xFF ? vm::read<u64>(reloc.ptr) : segments.at(reloc.idx_addr).addr + reloc.ptr;

                // TODO: clean up the bitfield syntax

                switch(reloc.type)
                {
                case 1:
                    vm::write<u32>(addr, data);
                    break;
                case 4:
                    vm::write<u16>(addr, data);
                    break;
                case 5:
                    vm::write<u16>(addr, data >> 16);
                    break;
                case 6:
                    vm::write<u16>(addr, (data >> 16) + (data & 0x8000 ? 1 : 0));
                    break;
                case 10:
                    vm::write<endian::big<bitrange<u32, 6, 30>>>(addr, bitrange<u32, 6, 30>(static_cast<u32>((data - addr) >> 2)));
                    break;
                case 11:
                    vm::write<endian::big<bitrange<u32, 16, 30>>>(addr, bitrange<u32, 16, 30>(static_cast<u32>((data - addr) >> 2)));
                    break;
                case 38:
                    vm::write<u64>(addr, data);
                    break;
                case 44:
                    vm::write<u64>(addr, data - addr);
                    break;
                case 57:
                    vm::write<endian::big<bitrange<u16, 0, 14>>>(addr, bitrange<u16, 0, 14>(static_cast<u16>(data >> 2)));
                    break;
                default:
                    spdlog::error("invalid relocation type {}", reloc.type);
                    break;
                }
            }
        }
        
        auto hash = XXH64_digest(hasher.get());
        spdlog::info("prx hash: {}", hash);

        struct library_info
        {
            big<u16> attrib;
            u8 version[2];
            char name[28];

            big<u32> toc;

            big<u32> symbols_front;
            big<u32> symbols_back;

            big<u32> deps_front;
            big<u32> deps_back;
        };

        auto info = vm::read<library_info>(segments[0].addr + mod.progs[0].paddress - mod.progs[0].offset);
        auto name = std::string(info.name);

        spdlog::info("name {}, version {}.{}", name, info.version[0], info.version[1]);

        auto symbols = load_symbols(info.symbols_front, info.symbols_back);
    }
}
