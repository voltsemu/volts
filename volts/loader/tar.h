#pragma once

#include <svl/file.h>

#include <map>

namespace vt::tar {
    using namespace svl;

    struct object {

        svl::file get(const std::string& path);

        void extract(const fs::path& dir);

        struct entry {
            u64 offset = 0;
            u64 size = 0;
        };

        std::map<std::string, entry> offsets;
        svl::file source;
    };

    object load(svl::file&& source);
}