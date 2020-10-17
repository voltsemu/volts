#include <svl/cmd.h>
#include <svl/log.h>

#include "loader/sfo.h"
#include "loader/pup.h"
#include "loader/tar.h"

#include <toml++/toml.h>
#include <fstream>
#include <sstream>

using namespace svl;
namespace l = svl::log;

#define EXPECT(expr, msg) if (!(expr)) { l::fatal(msg); std::exit(1); }

cmd::cli cli = cmd::cli("ps3 emulator tools", {
    cmd::str("output", "O", "redirect output to file", [](std::string str) {
        l::info("redirecting output to: {}", str);
        l::stream = std::make_unique<log::flog>(open(str, Mode::write));
    }),
    cmd::pos("help", "h", "print help message", [] {
        l::info(cli.help("vlt"));
    }),
    cmd::str("sfo-read", "SR", "read a .SFO", [](std::string path) {
        EXPECT(fs::exists(path), fmt::format("couldnt find sfo file {}", path))
        auto sfo = vt::sfo::load(open(path));
        EXPECT(sfo.has_value(), "failed to load SFO file")
        toml::table out;

        for (const auto& entry : sfo.value()) {
            auto key = entry.first;
            std::visit(visitor {
                [&](u32 i) { out.insert_or_assign(key, i); },
                [&](std::string s) {
                    // erase trailing null terminators
                    s.erase(s.find('\0'));
                    out.insert_or_assign(key, s);
                },
                [&](const std::vector<u8>& v) {
                    toml::array arr;
                    for (auto i : v)
                        arr.push_back(i);
                    out.insert_or_assign(key, std::move(arr));
                }
            }, entry.second);
        }

        std::stringstream ss;
        ss << out << std::endl;
        l::write(ss.str());
    }),
    cmd::str("sfo-write", "SW", "create a .SFO file from a toml file", [](std::string path) {
        EXPECT(fs::exists(path), "failed to find source")
        auto input = toml::parse_file(path);
        EXPECT(input, "failed to parse toml file")
        auto table = std::move(input).table();

        vt::sfo::data data;

        for (auto entry : table) {
            entry.second.visit([key = entry.first, &data](auto& e) {
                if constexpr (toml::is_number<decltype(e)>) {
                    int64_t num = e.as_integer()->get();
                    data[key] = (u32)num;
                } else if constexpr (toml::is_string<decltype(e)>) {
                    std::string str = e.as_string()->get();
                    data[key] = str;
                } else if constexpr (toml::is_array<decltype(e)>) {
                    std::vector<u8> arr;
                    auto in = *e.as_array();
                    for (auto& elem : in) {}
                    //    arr.push_back(elem.as_integer()->get());
                    data[key] = arr;
                } else {
                    EXPECT(false, "invalid data type in input")
                }
                (void)e;
            });
            //const auto& key = entry.first;
            //const auto& val = entry.second;
            /*val.visit([&](auto&& e) {
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
                    EXPECT(false, "invalid data type in input")
                }
            });*/
        }

        open("PARAM.SFO", Mode::write)
            .insert(vt::sfo::save(data));
    }),
    cmd::str("pup", "P", "extract a .PUP file", [](std::string path) {
        EXPECT(fs::exists(path), "pup file does not exist")
        auto pup = vt::pup::load(open(path));
        EXPECT(pup.has_value(), "failed to parse pup file")

        auto entry = pup.value().get(0x300);
        auto packed = vt::tar::load(std::move(entry));
    })
});

int main(int argc, const char** argv) {
    cli.parse(argc, argv);
}
