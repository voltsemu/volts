#include <svl/cmd.h>
#include <svl/log.h>

#include "loader/sfo.h"
#include "loader/pup.h"

#include <toml++/toml.h>
#include <fstream>
#include <sstream>

using namespace svl;

#define ENSURE(expr, msg) if (!(expr)) { log::fatal(msg); std::exit(1); }

cmd::cli cli = cmd::cli("ps3 emulator tools", {
    cmd::str("output", "O", "redirect output to file", [](std::string str) {
        log::info("redirecting output to: {}", str);
        log::stream = std::make_unique<log::flog>(open(str, Mode::write));
    }),
    cmd::pos("help", "h", "print help message", [] {
        log::info(cli.help("vlt"));
    }),
    cmd::str("sfo-read", "SR", "read a .SFO", [](std::string path) {
        ENSURE(fs::exists(path), fmt::format("couldnt find sfo file {}", path))
        auto sfo = vt::sfo::load(open(path));
        ENSURE(sfo.has_value(), "failed to load SFO file")
        toml::table out;

        for (auto& [key, val] : sfo.value()) {
            std::visit(visitor {
                [&](u32 i) { out.insert(key, i); },
                [&](std::string s) {
                    // erase trailing null terminators
                    s.erase(s.find('\0'));
                    out.insert(key, s);
                },
                [&](const std::vector<u8>& v) {
                    toml::array arr;
                    for (auto i : v)
                        arr.push_back(i);
                    out.insert(key, std::move(arr));
                }
            }, val);
        }

        std::stringstream ss;
        ss << out << std::endl;
        log::write(ss.str());
    }),
    cmd::str("sfo-write", "SW", "create a .SFO file from a toml file", [](std::string path) {
        ENSURE(fs::exists(path), "failed to find source")
        auto input = toml::parse_file(path);
        ENSURE(!!input, "failed to parse toml file")
        auto table = std::move(input).table();

        vt::sfo::data data;

        for (auto [key, val] : table) {
            val.visit([&](auto&& e) {
                if constexpr (toml::is_number<decltype(e)>) {
                    data[key] = val.as_integer()->get();
                } else if constexpr (toml::is_string<decltype(e)>) {
                    data[key] = val.as_string()->get();
                } else if constexpr (toml::is_array<decltype(e)>) {
                    std::vector<u8> arr;
                    for (auto& elem : *val.as_array())
                        arr.push_back(elem.as_integer()->get());
                    data[key] = arr;
                } else {
                    ENSURE(false, "invalid data type in input")
                }
            });
        }

        open("PARAM.SFO", Mode::write)
            .insert(vt::sfo::save(data));
    }),
    cmd::str("pup", "P", "extract a .PUP file", [](std::string path) {
        ENSURE(fs::exists(path), "pup file does not exist")
        auto pup = vt::pup::load(open(path));
        ENSURE(pup.has_value(), "failed to parse pup file")

        for (auto entry : pup->files) {
            (void)entry;
        }
    })
});

int main(int argc, const char** argv) {
    cli.parse(argc, argv);
}
