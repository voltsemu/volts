#include "module.h"

#include "vm/vm.h"

#include <xxhash.h>

namespace volts::ppu
{
    void load_module(loader::elf::ppu_prx& mod)
    {
        std::unique_ptr<XXH64_state_t, decltype(&XXH64_freeState)> hash(XXH64_createState(), XXH64_freeState);
        
        for(auto& prog : mod.progs)
        {

        }

        for(auto& sect : mod.sects)
        {

        }
    }
}