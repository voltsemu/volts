#include <volts.h>

// logging
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

// filesystem
#include <svl/wrapfs.h>
#include <svl/file.h>

#include <future>

#include "vm/vm.h"
#include "vm/ppu/module.h"
#include "vm/ppu/thread.h"

#include "elf.h"

#include "loader/sfo.h"
#include "crypt/self.h"
#include "crypt/sce.h"
#include "loader/pup.h"
#include "loader/tar.h"

#define CXXOPTS_NO_EXCEPTIONS
#define CXXOPTS_NO_RTTI

#include <cxxopts.hpp>

#include <toml++/toml.h>
#include <fstream>

#include "rsx/render.h"

namespace volts::cmd
{
    namespace opts = cxxopts;

    using namespace loader;
    using namespace crypt;

    namespace cvt = svl::convert;

    using namespace std::string_view_literals;


    fs::path extract_firmware(const fs::path& path, const config& conf)
    {
        spdlog::info("starting pup decryption");

        auto file = svl::open(path, svl::mode::read);
        auto pup = pup::load(file).expect("failed to load pup file");

        spdlog::info("starting pup extraction");

        auto tar = tar::load(pup.get_file(0x300).expect("failed to find file 0x300"));

        // create a vector of all decryption tasks
        // when this exits scope all the futures destructors
        // will be called and everything will be decrypted at the same time
        std::vector<std::future<void>> tasks = {};

        for(auto&& field : tar.offsets)
        {
            const auto key = field.first;

            if(key.rfind("dev_flash_", 0) != 0)
                continue;

            tasks.push_back(std::async(std::launch::async, [vfs = conf.vfs, name = key, file = tar.get_file(key.c_str())] {
                spdlog::info("decrypting pup entry {}", name);
                auto update = sce::load(file);

                update[2].seek(0);

                auto dec = tar::load(update[2]);

                spdlog::info("extracing entry into vfs");

                dec.extract(vfs);
            }));
        }
        spdlog::info("extracting {} entries", tasks.size());

        return conf.vfs/"dev_flash";
    }

    void parse(int argc, char** argv)
    {
        config conf = {};

        opts::Options opts = { "volts", "ps3 command line tools" };
        opts.add_options()
            ("help", "print help and exit")
            ("log-lvl", "set logging level", opts::value<std::string>())
            ("log-out", "set logging output", opts::value<std::string>())
            ("sfo", "parse an sfo file", opts::value<std::string>())
            ("sfo-write", "create an sfo file from a toml config", opts::value<std::string>())
            ("pup", "parse a pup file", opts::value<std::string>())
            ("self", "parse a self file", opts::value<std::string>())
            ("render", "enable rendering window", opts::value<std::string>())
            ("name", "set the game name", opts::value<std::string>())
            ("exec", "load a ps3 (s)elf executable", opts::value<std::string>())
            ("prx", "load a ps3 prx", opts::value<std::string>())
            ("boot", "boot a ps3 game from a directory", opts::value<std::string>())
            ("cfg", "path to toml config file to configure the emulator with", opts::value<std::string>())
            ;

        auto res = opts.parse(argc, argv);

        if(res.count("help"))
        {
            spdlog::info(opts.help());
            std::exit(0);
        }

        if(res.count("log-lvl"))
        {
            auto str = res["log-lvl"].as<std::string>();

            switch(str[0])
            {
            case '0': case 'o': case 'O':
                spdlog::info("disaling logging");
                spdlog::set_level(spdlog::level::off);
                break;
            case '1': case 'd': case 'D':
                spdlog::info("setting log level to debug");
                spdlog::set_level(spdlog::level::debug);
                break;
            case '2': case 'i': case 'I':
                spdlog::info("setting log level to info");
                spdlog::set_level(spdlog::level::info);
                break;
            case '3': case 'w': case 'W':
                spdlog::info("setting log level to warn");
                spdlog::set_level(spdlog::level::warn);
                break;
            case '4': case 'e': case 'E':
                spdlog::info("setting log level to error");
                spdlog::set_level(spdlog::level::err);
                break;
            case '5': case 'c': case 'C':
                spdlog::info("setting log level to critical");
                spdlog::set_level(spdlog::level::critical);
                break;
            default:
                spdlog::warn("invalid log level {}. must be one of [off | debug | info | warn | err | critical]", str);
                break;
            }
        }

        if(res.count("cfg"))
        {
            if(auto path = res["cfg"].as<std::string>(); fs::exists(path))
            {
                auto cfg = toml::parse_file(path);

                conf.aes = cfg["global"]["aes-ni"].as_boolean()->value_or(false);
                conf.vfs = fs::path(cfg["global"]["vfs"].as_string()->value_or("vfs"));
                conf.render = cfg["global"]["render"].value_or("vulkan"sv);

                fs::create_directories(conf.vfs);
            }
            else
            {
                spdlog::error("cannot find config file {}", path.c_str());
            }
        }
        else
        {
            conf.aes = false;
            conf.vfs = "vfs";
            conf.render = "vulkan";
        }

        fs::create_directories(conf.vfs);

        if(res.count("log-out"))
        {
            auto path = res["log-out"].as<std::string>();
            spdlog::set_default_logger(spdlog::basic_logger_mt("volts", path));
            spdlog::info("changed logging output file");
        }

        if(res.count("sfo-write"))
        {
            if(auto path = res["sfo-write"].as<std::string>(); fs::exists(path))
            {
                auto input = toml::parse_file(path);

                sfo::object obj;
                for(auto&& [key, val] : input)
                {
                    if(val.is_string())
                    {
                        auto str = val.as_string()->get();
                        spdlog::info("data {}\n", str);
                        std::vector<svl::byte> data(str.begin(), str.end());
                        data.push_back('\0');

                        obj[key] = { sfo::format::string, data };
                    }
                    else if(val.is_integer())
                    {
                        union {
                            int64_t v;
                            svl::byte b[8];
                        } data = { val.as_integer()->get() };

                        std::vector<svl::byte> vec;
                        vec.push_back(data.b[0]);
                        vec.push_back(data.b[1]);
                        vec.push_back(data.b[2]);
                        vec.push_back(data.b[3]);

                        obj[key] = { sfo::format::integer, vec };
                    }
                    else if(val.is_array())
                    {
                        std::vector<svl::byte> data;
                        for(auto&& v : *val.as_array())
                        {
                            if(!v.is_integer())
                            {
                                spdlog::warn("invalid data in array at key {}", key);
                            }

                            data.push_back((svl::byte)v.as_integer()->get());
                        }
                    }
                    else
                    {
                        spdlog::warn("invalid type for key {}", key);
                    }
                }

                auto output = svl::open("PARAM.SFO", svl::mode::write);
                sfo::write(output, obj);
            }
            else
            {
                spdlog::error("cannot find input file {}", path);
            }

        }

        if(res.count("boot"))
        {
            if(fs::path dir = res["boot"].as<std::string>(); fs::exists(dir))
            {
                auto firmware = dir/"PS3_UPDATE"/"PS3UPDAT.PUP";
                auto game = dir/"PS3_GAME"/"USRDIR"/"EBOOT.BIN";
                auto desc = dir/"PS3_GAME"/"PARAM.SFO";

                // TODO: ask spdlog to add a formatter for std::filesystem::path (and to undelete the meson build)
                spdlog::info("loading firmware {}", firmware.string());
                spdlog::info("loading game {}", game.string());
                spdlog::info("loading description {}", desc.string());

                auto param = sfo::load(svl::open(desc, svl::mode::read))
                    .expect("failed to read param data");

                auto dev_flash = extract_firmware(firmware, conf);
                auto sprx = dev_flash/"sys"/"external"/"liblv2.sprx";

                spdlog::info("decrypting liblv2 from {}", fs::absolute(sprx).string());
                auto f = svl::open(sprx, svl::mode::read);
                auto liblv2 = self::load(f).expect("failed to load liblv2");

                auto elf = elf::load<elf::ppu_prx>(std::move(liblv2)).value();
                ppu::load_prx(elf);

                spdlog::info("booting game {} - {}", param["TITLE"].as<std::string>(), param["VERSION"].as<std::string>());
            }
            else
            {
                spdlog::critical("failed to find {}", dir.string());
            }
        }

        if(res.count("sfo"))
        {
            if(fs::path path = res["sfo"].as<std::string>(); fs::exists(path))
            {
                auto obj = sfo::load(svl::open(path, svl::mode::read))
                    .expect("failed to parse sfo file");

                auto out = svl::open("param.toml", svl::mode::write);

                for(auto&& [key, val] : obj)
                {
                    out.write(key + " = ");

                    switch(val.type)
                    {
                    case sfo::format::integer:
                        out.write(std::to_string(*(int*)val.data.data()));
                        break;
                    case sfo::format::string:
                        out.write(fmt::format("\"{}\"", std::string((char*)val.data.data())));
                        break;
                    case sfo::format::array:
                        out.write("[ ");
                        for(size_t i = 0; i < val.data.size(); i++)
                        {
                            if(i != 0)
                                out.write(", ");

                            out.write(std::to_string(val.data[i]));
                        }
                        out.write("]");
                        break;
                    }

                    out.write(std::string("\n"));
                }

                spdlog::info("write parsed file to param.toml");
            }
            else
            {
                spdlog::error("the file {} does not exist", path.string());
            }
        }

        if(res.count("pup"))
        {
            if(fs::path path = res["pup"].as<std::string>(); fs::exists(path))
            {
                extract_firmware(path, conf);
            }
            else
            {
                spdlog::error("no pup file found at {}", path.string());
            }
        }

        if(res.count("self"))
        {
            if(fs::path path = res["self"].as<std::string>(); fs::exists(path))
            {
                auto self = self::load(svl::open(path, svl::mode::read))
                    .expect("failed to decrypt self");
                self.save("eboot.elf");
                spdlog::info("saved decrypted elf to eboot.elf");
            }
            else
            {
                spdlog::error("no self file found at {}", path.string());
            }
        }

        if(res.count("prx"))
        {
            svl::file f = svl::open(res["prx"].as<std::string>(), svl::mode::read);

            auto elf = (f.read<svl::u32>() == cvt::to_u32("\177ELF")) ? f : self::load(f).expect("failed to decrypt self");
            auto prx = elf::load<elf::ppu_prx>(std::move(elf))
                .expect("failed to load (s)elf file");

            vm::init();

            ppu::load_prx(prx);
        }

        if(res.count("exec"))
        {
            svl::file f = svl::open(res["exec"].as<std::string>(), svl::mode::read);

            auto elf = (f.read<svl::u32>() == cvt::to_u32("\177ELF")) ? f : self::load(f).expect("failed to decrypt self");
            auto exec = elf::load<elf::ppu_exec>(std::move(f))
                .expect("failed to load (s)elf file");

            vm::init();

            ppu::load_exec(exec);
        }

#if RENDER
        if(res.count("render"))
            volts::rsx::run(res["render"].as<std::string>().c_str(), res.count("name") ? res["name"].as<std::string>().c_str() : "volts", "1.00");
#endif
    }
}

int main(int argc, char** argv)
{
#if SYS_WINDOWS
    // UTF-8 console output for windows
    SetConsoleOutputCP(CP_UTF8);
    setvbuf(stdout, nullptr, _IOFBF, 1024);
#endif
    volts::cmd::parse(argc, argv);
}
