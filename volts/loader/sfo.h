#pragma once

#include <svl/types.h>
#include <svl/file.h>

#include <variant>
#include <string>
#include <vector>
#include <optional>
#include <map>

namespace vt::sfo {
    using namespace svl;

    using entry = std::variant<
        u32, 
        std::string, 
        std::vector<u8>
    >;

    using data = std::map<std::string, entry>;

    std::optional<data> load(svl::file&& file);
    svl::file save(const data& items);
}
