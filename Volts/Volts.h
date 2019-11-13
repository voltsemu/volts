#pragma once

#include "Volts/Core/Emulator.h"

#include <filesystem>
#include <fstream>
#include <future>
#include <atomic>

#include "Volts/Utils/SFO.h"
#include "Volts/Utils/UNSELF.h"
#include "Volts/Utils/PUP.h"
#include "Volts/Utils/TAR.h"

#define CXXOPTS_NO_EXCEPTIONS
#define CXXOPTS_NO_RTTI

#include "cxxopts/cxxopts.h"

#include <argo.hpp>

#include <string>

#if OS_WINDOWS
#   include <io.h>
#   include <fcntl.h>
#endif

namespace Volts::Args
{
    using namespace Cthulhu;

    using namespace Utils;

    using namespace std;

    namespace fs = std::filesystem;

    struct CLI
    {
        cxxopts::Options Opts;

        CLI()
            : Opts("volt", "Command line emulator interface")
        {
#if OS_WINDOWS
            if(AttachConsole(ATTACH_PARENT_PROCESS))
            {
                freopen("CONOUT$", "w", stdout);
                freopen("CONOUT$", "w", stderr);
            }
#endif
        }

        static CLI* Get()
        {
            static CLI* Singleton = new CLI();
            return Singleton;
        }

        CLI* Build()
        {
            Opts.add_options()
                ("H,help", "Display help message then exit")
                ("V,version", "Print version")
                ("N,nogui", "Run all other command line tasks then exit before creating a gui")
                ("P,pup", "Parse PS3UPDAT.PUP file", cxxopts::value<std::string>())
                ("L,level", "Change logging verbosity", cxxopts::value<std::string>()->default_value("info"))
                ("R,run", "Play the game at location", cxxopts::value<std::string>())
                ("S,sfo", "Parse a SFO and dump to json", cxxopts::value<std::string>())
                ("U,unself", "Decrypt a SELF and write to a file", cxxopts::value<std::string>())
                ("O,output", "A location to output data to", cxxopts::value<std::string>());
            return this;
        }

        void Run(int Argc, char** Argv)
        {
            auto Res = Opts.parse(Argc, Argv);

            if(Res.count("output"))
            {
                auto Path = Res["output"].as<std::string>();

                static auto Pipe = std::ofstream();
                Pipe.open(Path, std::ofstream::out);
                Emulator::Get()->OutStream = &Pipe;
            }

            {
                auto Level = Res["level"].as<std::string>();
                std::for_each(Level.begin(), Level.end(), [](char& C) {
                    C = ::tolower(C);
                });

                if(Level == "trace")
                    Emulator::Get()->Level = LogLevel::Trace;
                else if(Level == "info")
                    Emulator::Get()->Level = LogLevel::Info;
                else if(Level == "warn")
                    Emulator::Get()->Level = LogLevel::Warn;
                else if(Level == "error")
                    Emulator::Get()->Level = LogLevel::Error;
                else if(Level == "fatal")
                    Emulator::Get()->Level = LogLevel::Fatal;
                else
                    VWARN("Invalid --level flag, must be one of (trace, info, warn, error, fatal). Defaulting to info");
            }

            if(Res.count("version"))
            {
                argo::json Obj{argo::json::object_e};
                Obj["commit"] = GIT_COMMIT;
                Obj["branch"] = GIT_BRANCH;
                argo::unparser::unparse(*Emulator::Get()->OutStream, Obj, "", "", "", 1);
            }

            if(Res.count("pup"))
            {
                auto Path = Res["pup"].as<std::string>();
                if(!fs::exists(Path))
                {
                    VFATAL("PUP file at {} not found", Path);
                    exit(1);
                }

                FS::BufferedFile F = {Path.c_str()};

                auto PUP = Utils::LoadPUP(&F);

                if(!PUP.has_value())
                {
                    VFATAL("Failed to parse PUP file");
                    exit(1);
                }

                auto File = PUP->GetFile(0x300);
                auto Data = Utils::LoadTAR(File);
                if(Data.Offsets.size() == 0)
                {
                    VERROR("Failed to load TAR 0x300");
                    exit(1);
                }

                std::atomic<U32> Progress = 32;

                for(auto& [Name, Offset] : Data.Offsets)
                {
                    if(Name.find("dev_flash_") != std::string::npos)
                        continue;

                    std::async(std::launch::async, [&Data, &Progress](U32 Offset) {
                        VINFO("{}", Offset);
                        Progress++;
                    }, Offset);
                }
            }

            if(Res.count("unself"))
            {
                auto Path = Res["unself"].as<std::string>();
                if(!fs::exists(Path))
                {
                    VFATAL("SELF as {} was not found", Path);
                    exit(1);
                }

                auto SELF = Utils::LoadSELF({Path.c_str()});

                if(SELF.Len() == 0)
                {
                    VFATAL("Failed to decrypt SELF");
                    exit(1);
                }

                std::ofstream Output("out.elf", ios::out | ios::binary);

                Output.write((const char*)SELF.GetData(), SELF.Len());

                Output.close();
            }

            if(Res.count("sfo"))
            {
                auto Path = Res["sfo"].as<std::string>();
                if(!fs::exists(Path))
                {
                    VFATAL("SFO File at {} was not found", Path);
                    exit(1);
                }

                auto SFO = Utils::LoadSFO({Path.c_str()});

                argo::json Output{argo::json::object_e};

                for(auto& [Key, Val] : SFO)
                {
                    switch(Val.Type)
                    {
                        case Format::String:
                            Output[Key] = (char*)Val.Data.data();
                            break;
                        case Format::Array:
                        {
                            argo::json V{argo::json::array_e};
                            for(auto& B : Val.Data)
                                V.append(B);

                            Output[Key] = V;
                            break;
                        }
                        case Format::Integer:
                            Output[Key] = *(I32*)Val.Data.data();
                            break;
                        default:
                            Output[Key] = nullptr;
                            break;
                    }
                }

                argo::unparser::unparse(*Emulator::Get()->OutStream, Output, "", "", "", 1);
            }

            if(Res.count("help"))
            {
                // print help then exit
                VINFO(Opts.help());
                exit(0);
            }

            if(Res.count("nogui"))
                exit(0);
        }
    };
}