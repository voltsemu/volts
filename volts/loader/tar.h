#pragma once

#include <svl/file.h>

#include <map>

namespace vt::tar {
    using namespace svl;

    struct object {

        svl::file get(const std::string_view& path);

        void extract(const fs::path& dir);
    private:
        std::map<std::string, u64> offsets;
        svl::file source;
    };

    object load(svl::file&& source);
}