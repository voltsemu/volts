#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include <svl/file.h>
#include <svl/types.h>
#include <svl/option.h>

namespace volts::sfo
{
    using Value = std::variant<
        std::string,
        std::vector<svl::byte>,
        svl::i32
    >;

    using Object = std::unordered_map<std::string, Value>;

    svl::Option<Object> load(svl::File&& source);
    svl::File unload(const Object& data);
}
