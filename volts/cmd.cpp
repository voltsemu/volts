#include "volts.h"

// quick string hashing
#include <external/fnv1a.h>

// logging
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

// filesystem
#include <external/wrapfs.h>
#include "vfs/vfs.h"
#include "svl/file.h"

#include "loader/sfo.h"
#include "loader/unself.h"
#include "loader/elf.h"
#include "loader/pup.h"
#include "loader/tar.h"

#include "vm/ppu/thread.h"
#include "vm/ppu/module.h"
#include "vm/vm.h"

#define CXXOPTS_NO_EXCEPTIONS
#define CXXOPTS_NO_RTTI

#include <external/cxxopts.hpp>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace volts::cmd
{
    namespace json = rapidjson;
    namespace opts = cxxopts;

    using namespace loader;

    void parse(int argc, char** argv)
    {
        opts::Options opts = { "volts", "ps3 command line tools" };
        opts.add_options()
            ("help", "print help and exit")
            ("vfs", "set vfs root directory", opts::value<std::string>())
            ("log-lvl", "set logging level", opts::value<std::string>())
            ("log-out", "set logging output", opts::value<std::string>())
            ("sfo", "parse an sfo file", opts::value<std::string>())
            ("pup", "parse a pup file", opts::value<std::string>())
            ("self", "parse a self file", opts::value<std::string>())
            ("boot", "boot the emulator");

        auto res = opts.parse(argc, argv);

        if(res.count("help"))
        {
            spdlog::info(opts.help());
            std::exit(0);
        }

        if(res.count("log-lvl"))
        {
            auto str = res["log-lvl"].as<std::string>();

            switch(hash_32_fnv1a_const(str.c_str()))
            {
            case hash_32_fnv1a_const("off"):
                spdlog::info("disaling logging");
                spdlog::set_level(spdlog::level::off);
                break;
            case hash_32_fnv1a_const("info"):
                spdlog::info("setting log level to info");
                spdlog::set_level(spdlog::level::info);
                break;
            case hash_32_fnv1a_const("warn"):
                spdlog::info("setting log level to warn");
                spdlog::set_level(spdlog::level::warn);
                break;
            case hash_32_fnv1a_const("err"):
                spdlog::info("setting log level to error");
                spdlog::set_level(spdlog::level::err);
                break;
            case hash_32_fnv1a_const("critical"):
                spdlog::info("setting log level to critical");
                spdlog::set_level(spdlog::level::critical);
                break;
            default:
                spdlog::warn("invalid log level {}. must be one of [off | info | warn | err | critical]", str);
                break;
            }
        }

        if(res.count("log-out"))
        {
            auto path = res["log-out"].as<std::string>();
            spdlog::set_default_logger(spdlog::basic_logger_mt("volts", path));
            spdlog::info("changed logging output file");
        }

        if(res.count("vfs"))
        {
            fs::path path = res["vfs"].as<std::string>();
            if(!fs::exists(path))
            {
                spdlog::info("path {} did not exist, creating required directories", path.string());
                fs::create_directories(path);
            }

            vfs::root(path);
            spdlog::info("updated vfs root to {}", fs::absolute(path).string());
        }

        if(res.count("sfo"))
        {
            if(fs::path path = res["sfo"].as<std::string>(); fs::exists(path))
            {
                auto file = svl::open(path, svl::mode::read);
                auto obj = sfo::load(file);

                if(!obj)
                {
                    spdlog::error("failed to parse sfo file");
                }
                else
                {
                    json::StringBuffer s;
                    json::Writer w(s);

                    w.StartObject();

                    for(auto& [key, val] : *obj)
                    {
                        w.Key(key.c_str());

                        switch(val.type)
                        {
                        case sfo::format::array:
                            w.StartArray();

                            for(auto b : val.data)
                                w.Int(b);

                            w.EndArray();
                            break;

                        case sfo::format::string:
                            w.String((char*)val.data.data());
                            break;
                        case sfo::format::integer:
                            w.Int(*(int*)val.data.data());
                            break;
                        default:
                            w.Null();
                            break;
                        }
                    }

                    w.EndObject();

                    spdlog::info(s.GetString());
                }
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
                spdlog::info("starting pup decryption");
                
                auto file = svl::open(path, svl::mode::read);
                auto pup = pup::load(file);

                spdlog::info("pup decryption finished");

                if(!pup)
                {
                    spdlog::error("failed to load pup file");
                }
                else
                {
                    spdlog::info("starting pup extraction");

                    auto tar = tar::load(pup->get_file(0x300));

                    for(auto& [key, off] : tar.offsets)
                    {
                        if(key.rfind("dev_flash_", 0) != 0)
                            continue;

                        spdlog::info("decrypting pup entry {}", key);

                        auto update = unself::load_sce(tar.get_file(key));

                        update[2].seek(0);

                        auto dec = tar::load(update[2]);

                        spdlog::info("extracing entry into vfs");

                        dec.extract(vfs::root());
                    }
                }
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
                auto self = unself::load_self(svl::open(path, svl::mode::read));
                self.save("eboot.elf");
                spdlog::info("saved decrypted elf to eboot.elf");
            }
            else
            {
                spdlog::error("no self file found at {}", path.string());
            }
        }

        if(res.count("boot"))
        {
            svl::file liblv2 = svl::open(vfs::get("dev_flash/sys/external/liblv2.sprx"), svl::mode::read);

            auto lib = unself::load_self(liblv2);

            auto elf = elf::load<elf::ppu_prx>(lib);

            vm::init();

            ppu::load_prx(elf.value());
            ppu::thread(elf->head.entry);
        }
    }
}