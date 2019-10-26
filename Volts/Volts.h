#pragma once

#include "Volts/Core/Emulator.h"

#include <filesystem>
#include <fstream>

#include "Volts/Utils/SFO.h"

#define CXXOPTS_NO_EXCEPTIONS
#define CXXOPTS_NO_RTTI

#include "cxxopts/cxxopts.h"

#include <argo.hpp>

#if OS_WINDOWS
#   include <io.h>
#   include <fcntl.h>
#endif

namespace Volts::Args
{
    using namespace Cthulhu;

    using namespace Utils;

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

        bool DisableGUI = false;

        CLI* Build()
        {
            Opts.add_options()
                ("H,help", "Display help message then exit")
                ("N,nogui", "Disable emulator gui")
                ("S,sfo", "Parse a SFO and dump to json", cxxopts::value<std::string>())
                ("U,unself", "Decrypt a SELF and write to a file", cxxopts::value<std::string>())
                ("O,output", "A location to output data to", cxxopts::value<std::string>());
            return this;
        }

        void Run(int Argc, char** Argv)
        {
            auto Res = Opts.parse(Argc, Argv);

            std::ostream* OutPipe = &std::cout;

            if(Res.count("output"))
            {
                auto Path = Res["output"].as<std::string>();

                static auto Pipe = std::ofstream();
                Pipe.open(Path, std::ofstream::out);
                OutPipe = &Pipe;
            }

            if(Res.count("help"))
            {
                // print help then exit
                *OutPipe << Opts.help();
                exit(0);
            }

            if(Res.count("sfo"))
            {
                auto Path = Res["sfo"].as<std::string>();
                if(!fs::exists(Path))
                {
                    *OutPipe << "SFO File at " << Path.c_str() << " was not found" << std::endl;
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

                argo::unparser::unparse(*OutPipe, Output, "", "\n", "    ", 1);
            }

            if(Res.count("nogui"))
                exit(0);
        }
    };
}