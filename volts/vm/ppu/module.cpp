#include "module.h"

#include "vm/vm.h"

#include <xxhash.h>

namespace volts::ppu
{
    void load_module(loader::elf::ppu_prx& mod)
    {
        std::unique_ptr<XXH64_state_t, decltype(&XXH64_freeState)> hasher(XXH64_createState(), XXH64_freeState);

        XXH64_reset(hasher.get(), 0);

        for(auto& prog : mod.progs)
        {
            XXH64_update(hasher.get(), reinterpret_cast<void*>(&prog.vaadres), sizeof(decltype(prog.vaadres)));
        }

        for(auto& sect : mod.sects)
        {
            XXH64_update(hasher.get(), reinterpret_cast<void*>(&sect.address), sizeof(decltype(sect.address)));
        }

        auto hash = XXH64_digest(hasher.get());
    }
}