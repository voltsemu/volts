#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <algorithm>
#include <fmt/core.h>

namespace svl::cmd {
    struct entry {
        std::string full;
        std::string brief;
        std::string_view desc;

        enum {
            pos,
            str,
            num
        } type = pos;

        void* callback;

        int apply(int argc, const char** argv, int i) {
            auto next = [=] {
                if (argc > i)
                    return argv[i + 1];
                else
                    return "";
            };

            switch (type) {
            case pos:
                ((void(*)())callback)();
                return 0;
            case str:
                ((void(*)(std::string))callback)(next());
                return 1;
            case num:
                ((void(*)(int))callback)(std::stoi(next()));
                return 1;
            default:
                return 0;
            }
        }
    };

    template<typename F>
    entry argument(decltype(entry::type) type, std::string full, std::string brief, std::string_view desc, F&& func) {
        using namespace std::literals;

        entry out = {
            "--"s + full,
            "-"s + brief,
            desc,
            type,
            (void*)func
        };

        return out;
    }

    entry str(std::string full, std::string brief, std::string_view desc, void(*func)(std::string)) {
        return argument(entry::str, full, brief, desc, func);
    }

    entry pos(std::string full, std::string brief, std::string_view desc, void(*func)()) {
        return argument(entry::pos, full, brief, desc, func);
    }

    struct cli {
        cli(std::string_view n, std::vector<svl::cmd::entry> e)
            : desc(n)
            , entries(e)
        { }

        std::string_view desc;
        std::vector<svl::cmd::entry> entries;

        std::vector<std::string> parse(int argc, const char** argv) {
            std::vector<std::string> out;

            for (int i = 1; i < argc; i++) {
                bool found = false;
                for (auto entry : entries) {
                    if (entry.full == argv[i] || entry.brief == argv[i]) {
                        i += entry.apply(argc, argv, i);
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    out.push_back(argv[i]);
                }
            }

            return out;
        }

        std::string help(const char* exe = "") {
            std::string out = fmt::format("{}: {}", exe, desc);

            int len = 0;

            for (auto entry : entries)
                len = std::max<int>(len, entry.full.length() + entry.brief.length() + 1);

            for (auto entry : entries) {
                auto name = (entry.full + " " + entry.brief);
                out += fmt::format("\n {}:{} {}", name, std::string(len - name.length(), ' '), entry.desc);
            }

            return out;
        }
    };
}