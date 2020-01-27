#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "vfs/vfs.h"

#include "loader/sfo.h"
#include "loader/unself.h"
#include "loader/elf.h"
#include "loader/pup.h"
#include "loader/tar.h"

#include "vm/ppu/thread.h"
#include "vm/ppu/module.h"
#include "vm/vm.h"

#include "svl/file.h"

#include <filesystem>

#define CXXOPTS_NO_EXCEPTIONS
#define CXXOPTS_NO_RTTI

#include <external/cxxopts.hpp>

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

namespace volts
{
    namespace fs = std::filesystem;
    namespace json = rapidjson;
    namespace opts = cxxopts;

    using namespace loader;

    struct cmd
    {
        static cmd& get()
        {
            static cmd* singleton = new cmd();
            return *singleton;
        }

        void parse(int argc, char** argv)
        {
            auto opts = build_options(argc, argv);

            if(opts.count("output"))
            {
                auto path = opts["output"].as<std::string>();
                auto log = spdlog::basic_logger_mt("volts", path);
                spdlog::set_default_logger(log);
                output_path = path;
            }

            if(opts.count("vfs"))
            {
                auto path = opts["vfs"].as<std::string>();
                std::filesystem::create_directories(path);
                vfs::set_root(path);
                spdlog::info("set vfs directory to {}", path);
            }

            if(opts.count("help"))
            {
                spdlog::info(options.help());
                std::exit(0);
            }

            if(opts.count("unself"))
            {
                if(auto path = opts["unself"].as<std::string>(); fs::exists(path))
                {
                    svl::file stream = svl::open(path, svl::mode::read);
                    auto obj = loader::unself::load_self(stream);

                    auto out = output_file("eboot.elf");
                    svl::file out_file = svl::open(vfs::get_path(out), svl::mode::write);
                    out_file.write(obj);
                    spdlog::info("decrypted self and wrote file to {}", fs::absolute(out).string());
                }
            }

            if(opts.count("pup"))
            {
                if(auto path = opts["pup"].as<std::string>(); fs::exists(path))
                {
                    spdlog::info("begining pup decryption");
                    auto stream = svl::open(path, svl::mode::read);
                    auto obj = loader::pup::load(stream);

                    if(!obj)
                    {
                        spdlog::error("failed to load pup file");
                        std::exit(1);
                    }

                    auto file = obj->get_file(0x300);
                    auto tar_file = loader::tar::load(file);
                    for(auto& [key, offset] : tar_file.offsets)
                    {
                        if(key.rfind("dev_flash_", 0) == 0)
                        {
                            auto update = tar_file.get_file(key);
                            auto update_dec = unself::load_sce(update);

                            update_dec[2].seek(0);

                            auto t = loader::tar::load(update_dec[2]);

                            t.extract(vfs::get_root());
                        }
                    }

                    spdlog::info("pup decryption finished");
                }
                else
                {
                    spdlog::error("no pup file found at {}", path);
                }
            }

            if(opts.count("load"))
            {
                svl::file firmware = svl::open(vfs::get_path("dev_flash/sys/external/liblv2.sprx"), svl::mode::read);

                spdlog::info("valid: {}", firmware.valid());
                auto dec = loader::unself::load_self(firmware);

                auto elf = loader::elf::load<loader::elf::ppu_prx>(dec);
                spdlog::info("loaded {}", elf.has_value());

                vm::init();

                ppu::load_prx(elf.value());
                spdlog::info("loaded liblv2 {}", elf->head.entry);
                ppu::thread(elf->head.entry);
            }

            if(opts.count("sfo"))
            {
                if(auto path = opts["sfo"].as<std::string>(); fs::exists(path))
                {
                    auto stream = svl::open(path, svl::mode::read);
                    auto obj = loader::sfo::load(stream);

                    json::StringBuffer s;
                    json::Writer w(s);

                    w.StartObject();
                
                    if(!obj)
                    {
                        w.Key("success");
                        w.Bool(false);
                    }
                    else
                    {
                        w.Key("success");
                        w.Bool(true);

                        for(auto& [key, val] : *obj)
                        {
                            w.Key(key.c_str());
                            switch(val.type)
                            {
                            case sfo::format::array:
                                w.StartArray();
                                
                                for(auto byte : val.data)
                                    w.Int(byte);

                                w.EndArray();
                                break;
                            case sfo::format::string:
                                w.String((char*)val.data.data());
                                break;
                            case sfo::format::integer:
                                w.Int(*(int*)val.data.data());
                                break;
                            default:
                                w.StartObject();
                                
                                w.Key("error");
                                w.String("unknown_type");

                                w.EndObject();
                                break;
                            }
                        }
                    }
                
                    w.EndObject();

                    auto f = svl::open("sfo.json", svl::mode::write);
                    f.write<svl::u8>(std::vector<svl::u8>({ 0xEF, 0xBB, 0xBF }));
                    f.write(s.GetString());

                    spdlog::info("wrote parsed sfo file to {}", fs::absolute("sfo.json").string());
                }
            }

            if(opts.count("live"))
                for(;;);
        }

    private:

        opts::Options options = {"volts", "ps3 emulator"};

        opts::ParseResult build_options(int argc, char** argv)
        {
            options.add_options()
                ("help", "print help and exit")
                ("output", "set logging output file", opts::value<std::string>())
                ("pup", "parse a .pup file", opts::value<std::string>())
                ("load", "load using firmware in the vfs")
                ("sfo", "parse an .sfo file", opts::value<std::string>())
                ("unself", "decrypt a self file", opts::value<std::string>())
                ("live", "stay alive")
            ;

            return options.parse(argc, argv);
        }

        std::string output_file(const std::string& name)
        {
            if(output_path)
            {
                auto ret = *output_path;
                ret.replace_extension("." + name);
                return ret.string();
            }
            
            return name;
        }

        std::optional<fs::path> output_path = std::nullopt;
    };
}