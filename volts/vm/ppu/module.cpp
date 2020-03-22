#include "module.h"

#include "vm.h"

#include <xxhash.h>

#include "vfs.h"

#include <spdlog/spdlog.h>

#include <map>

#include <bitrange.h>

#include "thread.h"

namespace volts::ppu
{
    using namespace svl;
    using namespace endian;

    struct module_info
    {
        u8 size;
        pad pad1[1];

        big<u16> version;
        big<u16> funcs;
        big<u16> vars;
        big<u16> tlsvars;
        u8 hash;
        u8 tlshash;

        pad pad2[2];

        big<u32> name;
        big<u32> nids;
        big<u32> addrs;
        big<u32> vnids;
        big<u32> vstubs;

        pad pad3[8];
    };

    std::map<u32, u32> load_exports(u32 begin, u32 end)
    {
        u32 addr = begin;
        while(addr < end)
        {
            auto lib = vm::read<module_info>(addr);

            if(!lib.name)
            {

            }
            else
            {
                spdlog::info("name {}", std::string(vm::ptr<char>(lib.name.get())));
            }
            
        }

        return {};
    }

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
            
            big<u32> unk3;
            big<u32> unk4;
        };

        std::map<u32, u32> symbols = {};

        auto addr = front;

        while(addr < back)
        {
            spdlog::info("offset {}", addr);
            spdlog::info("len = {}", vm::read<u8>(addr));
            spdlog::info("version = {}", vm::read<big<u16>>(addr + 2));

            auto info = vm::read<module_info>(addr);
            // Todo
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

            spdlog::info("loaded program data at {}", addr);
            
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
                        static_cast<u32>(sect.flags & 7),
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
                auto addr = vm::read<big<u32>>(segments.at(reloc.idx_addr).addr + reloc.offset);
                auto data = reloc.idx_val == 0xFF ? byte_swap(vm::read<u64>(reloc.ptr)) : segments.at(reloc.idx_addr).addr + reloc.ptr;

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
                    vm::write<u32>(addr, byte_swap(bitrange<u32, 7, 30>(static_cast<u32>((data - addr) >> 2)).read()));
                    break;
                case 11:
                    vm::write<u32>(addr, byte_swap(bitrange<u32, 17, 30>(static_cast<u32>((data - addr) >> 2)).read()));
                    break;
                case 38:
                    vm::write<u64>(addr, data);
                    break;
                case 44:
                    vm::write<u64>(addr, data - addr);
                    break;
                case 57:
                    vm::write<u16>(addr, byte_swap(bitrange<u16, 1, 14>(static_cast<u16>(data >> 2)).read()));
                    break;
                default:
                    spdlog::error("invalid relocation type {}", reloc.type);
                    break;
                }
            }

            break;
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

        spdlog::info("symbol range: {}-{}", info.symbols_front, info.symbols_back);

        auto symbols = load_symbols(info.symbols_front, info.symbols_back);
    }

    void load_exec(elf::ppu_exec& exec)
    {
        u32 pagesize = 0x100000;
        u32 stacksize = 0x100000;
        u32 segment = 0;
        for(auto prog : exec.progs)
        {
            // LOAD
            if(prog.type == 1)
            {
                if(!prog.mem_size)
                    continue;

                vm::addr addr = vm::any->falloc(prog.vaddress, prog.mem_size);

                exec.data.seek(prog.offset);
                auto data = exec.data.read<byte>(prog.file_size);

                std::memcpy(vm::base(addr), data.data(), data.size());
            }
            // TLS
            else if(prog.type == 7)
            {
            }
            // LOOS+1
            else if(prog.type == 0x60000001)
            {
                if(!prog.file_size)
                    continue;

                struct process_data
                {
                    big<u32> size;
                    big<u32> magic;
                    big<u32> version;
                    big<u32> sdk;
                    big<u32> priority;
                    big<u32> stacksize;
                    big<u32> pagesize;
                    big<u32> segment;

                    pad padding[4];
                };

                auto info = vm::read<process_data>(prog.vaddress);

                if(info.size != sizeof(process_data))
                {
                    spdlog::error("invalid process data size {}", info.size);
                }

                if(info.magic != 0x13BCC5f6)
                {
                    spdlog::error("invalid process magic {:x}", info.magic);
                }

                stacksize = info.stacksize;
                segment = info.segment;
                pagesize = info.pagesize;
            }
            // LOOS+2
            else if(prog.type == 0x60000002)
            {
                struct proc_param
                {
                    big<u32> size;
                    big<u32> magic;
                    big<u32> version;
                    
                    pad pad1[4];
                    
                    big<u32> libent_begin;
                    big<u32> libent_end;

                    big<u32> libstub_begin;
                    big<u32> libstub_end;
                };

                if(!prog.file_size)
                    continue;

                auto param = vm::read<proc_param>(prog.vaddress);

                if(param.magic != 0x1B434CEC)
                {
                    spdlog::error("invalid parameter magic {:x}", param.magic.get());
                }

                auto modules = load_exports(param.libent_begin, param.libent_end);
            }
            // oh no
            else
            {
                spdlog::critical("invalid elf section type {:x}", prog.type);
            }
        }

        switch(stacksize)
        {
        case 0x10: stacksize = 32 * 1024; break;
        case 0x20: stacksize = 64 * 1024; break;
        case 0x30: stacksize = 96 * 1024; break;
        case 0x40: stacksize = 128 * 1024; break;
        case 0x50: stacksize = 256 * 1024; break;
        case 0x60: stacksize = 512 * 1024; break;
        case 0x70: stacksize = 1024 * 1024; break;
        default:
            stacksize = std::clamp<u32>(stacksize, 0x10000, 0x100000);
            break;
        }

        ppu::thread(exec.head.entry);
    }
}
