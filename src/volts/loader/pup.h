#pragma once

#include <svl/endian.h>
#include <svl/file.h>
#include <svl/option.h>
#include <svl/platform.h>

namespace volts::pup
{
    namespace endian = svl::endian;

    struct Entry
    {
        endian::Big<svl::u64> id;
        endian::Big<svl::u64> offset;
        endian::Big<svl::u64> length;
        svl::pad<8> padding;
    };

    struct Hash
    {
        endian::Big<svl::u64> id;
        svl::byte hash[20];
        svl::pad<4> padding;
    };

    struct Object
    {
        svl::Option<svl::File> get(svl::u64 id);

        std::vector<Entry> files;
        std::vector<Hash> hashes;
        svl::File source;
    };

    VISIBLE svl::Option<Object> load(svl::File&& source);
} // namespace volts::loader::pup