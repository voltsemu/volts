#pragma once

#include <svl/file.h>

namespace vt::pup {
    using namespace svl;

    struct entry {
        big<u64> id;
        big<u64> addr;
        big<u64> len;
        u8 pad[8];
    };

    struct hash {
        big<u64> id;
        u8 hash[20];
        u8 pad[4];
    };

    struct object {
        object(svl::file&& file)
            : source(std::move(file))
        { }

        std::vector<entry> files;
        std::vector<hash> hashes;

        svl::file get(u64 id);

    private:
        svl::file source;
    };

    std::optional<object> load(svl::file&& source);
}
