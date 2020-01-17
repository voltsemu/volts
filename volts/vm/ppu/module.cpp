#include "module.h"

#include "vm/vm.h"

#include <xxhash.h>

#include "vfs/vfs.h"

namespace volts::ppu
{
    // TODO: all this
    void load_prx(loader::elf::ppu_prx& mod)
    {
        std::unique_ptr<XXH64_state_t, decltype(&XXH64_freeState)> hasher(XXH64_createState(), XXH64_freeState);

        XXH64_reset(hasher.get(), 0);

        for(auto& prog : mod.progs)
        {
            XXH64_update(hasher.get(), reinterpret_cast<void*>(&prog.vaddress), sizeof(decltype(prog.vaddress)));
            
            spdlog::info("loading program header");

            if(prog.type == 0x700000A4)
                continue;

            mod.data->seek(prog.offset);
            auto data = svl::read_n(*mod.data, prog.file_size);

            spdlog::info("read in section {} {}", prog.offset, data.size());

            std::memcpy(vm::real(prog.vaddress), data.data(), data.size());
        }

        for(auto& sect : mod.sects)
        {
            XXH64_update(hasher.get(), reinterpret_cast<void*>(&sect.address), sizeof(decltype(sect.address)));

            spdlog::info("loading section header");

            mod.data->seek(sect.offset);
            auto data = svl::read_n(*mod.data, sect.size);
            std::memcpy(vm::real(sect.address), data.data(), data.size());
        }

        auto hash = XXH64_digest(hasher.get());
        spdlog::info("prx hash: {}", hash);
    }
}