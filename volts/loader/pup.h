#pragma once

#include <vector>
#include <optional>
#include <memory>

#include "svl/endian.h"
#include "svl/types.h"
#include "svl/file.h"

namespace volts::loader::pup
{
    struct entry
    {
        svl::endian::big<svl::u64> id;
        svl::endian::big<svl::u64> offset;
        svl::endian::big<svl::u64> length;
        svl::pad padding[8];
    };

    static_assert(sizeof(entry) == 32);

    struct hash
    {
        svl::endian::big<svl::u64> id;
        svl::byte hash[20];
        svl::pad padding[4];
    };

    static_assert(sizeof(hash) == 32);

    struct object
    {
        std::vector<entry> files;
        std::vector<hash> hashes;

        svl::file get_file(svl::u64 id);

        object(svl::file s)
            : file(s)
        {}

    private:
        svl::file file;
    };

    std::optional<object> load(svl::file stream);
}