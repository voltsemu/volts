#pragma once

#include <istream>
#include <map>
#include <string>

#include "svl/types.h"

namespace volts::loader::tar
{
    struct object
    {
        std::map<std::string, svl::u64> offsets;
        
        object(std::shared_ptr<std::istream> s)
            : file(s)
        {}

    private:
        std::shared_ptr<std::istream> file;
    };

    object load(std::shared_ptr<std::istream> stream);
}