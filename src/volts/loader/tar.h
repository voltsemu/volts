#pragma once

#include <unordered_map>
#include <string>

#include <svl/file.h>
#include <svl/types.h>
#include <svl/option.h>

namespace volts::tar
{
    using OffsetMap = std::unordered_map<std::string, svl::u64>;

    struct Object
    {
        svl::Option<svl::File> get(const std::string& str);
        void extract(const fs::path& path);

        OffsetMap offsets;
        svl::File source;
    };

    Object load(svl::File&& source);
}