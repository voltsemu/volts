#pragma once

#include <map>
#include <string>

#include "svl/types.h"
#include "svl/file.h"

namespace volts::loader::tar
{
    struct object
    {
        std::map<std::string, svl::u64> offsets;
        
        object(svl::file s)
            : file(s)
        {}

        svl::file get_file(std::string name);

        void extract(const std::filesystem::path& to);

    private:
        svl::file file;
    };

    object load(svl::file stream);
}