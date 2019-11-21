#pragma once

#include <map>
#include <string>

#include "svl/types.h"
#include "svl/stream.h"

namespace volts::loader::tar
{
    struct object
    {
        std::map<std::string, svl::u64> offsets;
        
        object(std::shared_ptr<svl::iostream> s)
            : file(s)
        {}

    private:
        std::shared_ptr<svl::iostream> file;
    };

    object load(std::shared_ptr<svl::iostream> stream);
}