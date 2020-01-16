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
            // std::memcpy(vm::base(prog.vaddress));
        }

        for(auto& sect : mod.sects)
        {
            XXH64_update(hasher.get(), reinterpret_cast<void*>(&sect.address), sizeof(decltype(sect.address)));
        }

        auto hash = XXH64_digest(hasher.get());
        spdlog::info("prx hash: {}", hash);
    }
}