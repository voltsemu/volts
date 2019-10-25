#include "Config.h"

#include "Emulator.h"

#include <iostream>
#include <fstream>

namespace Volts
{
    Config::Config()
    {
        ConfigDir = fs::current_path() / "Config";
        fs::create_directory(ConfigDir);

        ConfigPath = ConfigDir/"Config.json";

        if(!fs::exists(ConfigPath))
        {
            std::ofstream Out(ConfigPath.string());
            Out << "{}" << std::endl;
            Out.close();
        }
    }

    void Config::Init()
    {
        VINFO("Current config path {}", ConfigPath.string());

        Reload();

        if(!Data.has("log_level"))
            Data["log_level"] = "info";

        std::string Level = (std::string)Data["log_level"];
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
        {
            Emulator::Get()->Level = LogLevel::Info;
            VINFO("LogLevel in config was invalid, setting to default level");
            Data["log_level"] = "info";
        }

        VINFO("Current machine hash {}", MachineHash);

        if(UsingLocalConfig())
            VINFO("Config is local")
        else
            VWARN("Config was not generated on this machine")

        Save();
    }

    int Config::GetAspectRatio()
    {
        if(!Data.has("aspect"))
            SetAspectRatio("16:9");

        if(auto S = (std::string)Data["aspect"]; S != "16:9" && S != "4:3")
        {
            VERROR("Aspect ratio in config file was not 16:9 or 4:3, changing back to 16:9 default");
            SetAspectRatio("16:9");
        }

        return ((std::string)Data["aspect"] == "4:3") ? 0 : 1;
    }

    void Config::SetAspectRatio(std::string N)
    {
        Data["aspect"] = N;
        Save();
    }

    void Config::Reload()
    {
        auto Dat = argo::parser::load(ConfigPath.string());
        Data = *Dat.get();
        VINFO("Parsed config and reloaded data");
    }

    void Config::Save()
    {
        argo::unparser::save(Data, ConfigPath.string(), "", "\n", "    ", 1);
        VINFO("Saved config data to file");
    }
}