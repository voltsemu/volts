// logging
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

// filesystem
#include <wrapfs.h>
#include <file.h>
#include <vfs.h>

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

#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

#include "rsx/render.h"

namespace volts::cmd
{
    namespace json = rapidjson;
    namespace opts = cxxopts;

    using namespace loader;
    using namespace crypt;

    namespace cvt = svl::convert;

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
            ("render", "enable rendering window", opts::value<std::string>())
            ("name", "set the game name", opts::value<std::string>())
            ("exec", "load a ps3 executable", opts::value<std::string>())
            ("prx", "load a ps3 prx", opts::value<std::string>())
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
                auto obj = sfo::load(file).expect("failed to parse sfo file");

                json::StringBuffer s;
                json::Writer w(s);

                w.StartObject();

                for(auto& [key, val] : obj)
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
                auto pup = pup::load(file).expect("failed to load pup file");

                spdlog::info("starting pup extraction");

                auto tar = tar::load(pup.get_file(0x300).expect("failed to find file 0x300"));

                // create a vector of all decryption tasks
                // when this exits scope all the futures destructors
                // will be called and everything will be decrypted at the same time
                std::vector<std::future<void>> tasks = {};

                for(auto& [key, _] : tar.offsets)
                {
                    if(key.rfind("dev_flash_", 0) != 0)
                        continue;

                    tasks.push_back(std::async(std::launch::async, [name = key, file = tar.get_file(key)]{
                        spdlog::info("decrypting pup entry {}", name);
                        auto update = sce::load(file);

                        update[2].seek(0);

                        auto dec = tar::load(update[2]);
                        
                        spdlog::info("extracing entry into vfs");

                        dec.extract(vfs::root());
                    }));
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

            auto prx = (f.read<svl::u32>() == cvt::to_u32("\177ELF") 
                ? elf::load<elf::ppu_prx>(f)
                : elf::load<elf::ppu_prx>(self::load(f).expect("failed to decrypt self")))
                    .expect("failed to load (s)elf file");

            vm::init();

            ppu::load_prx(prx);
        }

        if(res.count("exec"))
        {
            svl::file f = svl::open(res["exec"].as<std::string>(), svl::mode::read);

            auto exec = (f.read<svl::u32>() == cvt::to_u32("\177ELF") 
                ? elf::load<elf::ppu_exec>(f)
                : elf::load<elf::ppu_exec>(self::load(f).expect("failed to decrypt self")))
                    .expect("failed to load (s)elf file");

            vm::init();

            ppu::load_exec(exec);
        }

        if(res.count("render"))
            volts::rsx::run(res["render"].as<std::string>(), res.count("name") ? res["name"].as<std::string>() : "volts");
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
