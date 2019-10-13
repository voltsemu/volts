#include "Emulator.h"

#include <iostream>
#include <filesystem>

namespace Volts
{
    using namespace std;
    using namespace rapidjson;
    Emulator* Emulator::Get()
    {
        static Emulator* Singleton = new Emulator();
        return Singleton;
    }

    void Emulator::Run()
    {
        /*if(!filesystem::is_directory("Config") || !filesystem::exists("Config"))
            filesystem::create_directory("Config");

        ifstream ConfigFile("Config/Config.json");
        Config.ParseStream<0>(ConfigFile);*/

        Config.Parse(R"(
            {
                "name": "jeff"
            }
        )");
    }
}