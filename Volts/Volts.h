#pragma once

#include "Volts/Core/Emulator.h"

#define CXXOPTS_NO_EXCEPTIONS
#define CXXOPTS_NO_RTTI

#include "cxxopts/cxxopts.h"

namespace Volts::Args
{   
    struct CLI
    {
        cxxopts::Options Opts;

        CLI()
            : Opts("Volts", "Command line emulator interface")
        {}

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
                ("S,sfo", "Parse a SFO and dump to json", cxxopts::value<std::string>());
            return this;
        }

        void Run(int Argc, char** Argv)
        {
            auto Res = Opts.parse(Argc, Argv);
            if(Res.count("sfo"))
            {
                auto Path = Res["sfo"].as<std::string>();
            }

            if(Res.count("help"))
            {
                // print help then exit
                printf("%s", Opts.help().c_str());
                exit(0);
            }

            if(Res.count("nogui"))
            {
                exit(0);
            }
        }
    };
}