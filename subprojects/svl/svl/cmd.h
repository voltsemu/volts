#pragma once

#include <string>
#include <vector>
#include <variant>
#include <optional>

#include "file.h"
#include "types.h"

#include <fmt/core.h>

namespace svl::cmd {
    struct entry {
        enum arg_type {
            // string
            str,

            // filesystem path
            path,

            // integer
            num,

            // positional argument or bool
            pos
        };

        const char* full;
        const char* brief;
        const char* desc;

        // type erased function pointer callback
        void* callback;

        // default value
        std::variant<std::string, int, bool, fs::path, std::monostate> fallback;

        arg_type type;

        template<typename F>
        constexpr entry& operator=(F&& func) {
            callback = (void*)func;
            return *this;
        }
    };

    template<typename T>
    constexpr entry arg(
        const char* name,
        const char* brief,
        const char* desc,
        std::optional<T> fallback = std::nullopt
    ) {
        entry out = { name, brief, desc, nullptr };

        if (fallback)
            out.fallback = fallback.value();
        else
            out.fallback = std::monostate();

        if constexpr (std::is_same_v<T, bool>) {
            out.type = entry::pos;
        } else if constexpr (std::is_same_v<T, std::string>) {
            out.type = entry::str;
        } else if constexpr (std::is_same_v<T, int>) {
            out.type = entry::num;
        } else if constexpr (std::is_same_v<T, fs::path>) {
            out.type = entry::path;
        } else {
            static_assert(false);
        }

        return out;
    }

    template<typename T>
    constexpr entry arg(
        const char* name,
        const char* desc,
        std::optional<T> fallback = std::nullopt
    ) { return arg(name, name, desc, fallback); }

    struct cli {
        cli(const char* d)
            : desc(d)
        { }

        cli& operator()(std::vector<entry>&& a) {
            args = a;
            return *this;
        }

        std::string help() const {
            auto header = fmt::format("{}: {}", argv[0], desc);
            return header;
        }

        std::vector<entry> args = {};

        const char* desc;

        struct iter {
            int idx;
            int argc;
            const char** argv;
        };

        iter parse(int n, const char** s) {
            argc = n;
            argv = s;

            return { 1, argc, argv };
        }

        int argc;
        const char** argv;
    };
}
