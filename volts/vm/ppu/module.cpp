#include "module.h"

#include "vm/vm.h"

#include <xxhash.h>

#include "vfs/vfs.h"

namespace volts::ppu
{
    // TODO: all this
    void load_prx(loader::elf::ppu_prx& mod)
    {
        spdlog::info("hasher");
        std::unique_ptr<XXH64_state_t, decltype(&XXH64_freeState)> hasher(XXH64_createState(), XXH64_freeState);

        spdlog::info("aaaa");

        XXH64_reset(hasher.get(), 0);

        spdlog::info("honk");

        for(auto& prog : mod.progs)
        {
            XXH64_update(hasher.get(), reinterpret_cast<void*>(&prog.vaadres), sizeof(decltype(prog.vaadres)));
        }

        for(auto& sect : mod.sects)
        {
            XXH64_update(hasher.get(), reinterpret_cast<void*>(&sect.address), sizeof(decltype(sect.address)));
        }

        auto hash = XXH64_digest(hasher.get());
        spdlog::info("prx hash: {}", hash);
    }
}