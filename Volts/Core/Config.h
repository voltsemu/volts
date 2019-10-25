#pragma once

#include <filesystem>

#include <argo.hpp>

#include <machineid/machineid.h>

#include <Meta/Aliases.h>

namespace Volts
{
    using namespace Cthulhu;
    namespace fs = std::filesystem;

    static std::string MachineHash = machineid::machineHash();

    struct Config
    {
        Config();

        void Init();

        void Reload();
        void Save();

        int GetAspectRatio();
        void SetAspectRatio(std::string NewRatio);

        bool UsingLocalConfig()
        {
            if(!Data.has("machid"))
            {
                Data["machid"] = MachineHash;
                Save();
                return false;
            }

            return (std::string)Data["machid"] == MachineHash;
        }

        bool GetVSync()
        {
            if(!Data.has("vsync"))
                UpdateVSync(false);

            return (bool)Data["vsync"];
        }

        void UpdateVSync(bool New)
        {
            Data["vsync"] = New;
            Save();
        }

        double GetGuiScale()
        {
            if(!Data.has("gui_scale"))
                UpdateGuiScale(1.0);

            return (double)Data["gui_scale"];
        }

        void UpdateGuiScale(double D)
        {
            Data["gui_scale"] = D;
            Save();
        }

        fs::path ConfigPath{};
        fs::path ConfigDir{};

        argo::json Data;
    };

    struct Emulator;

    namespace Cfg
    {
        template<typename T, const char* TName>
        struct Var
        {
            Var(T Def) : Default(Def) {}

            T operator T()
            {
                auto Cfg = Emulator::Get()->Cfg;
                if(!Cfg.Data.has(TName))
                {
                    Cfg.Data[TName] = Default;
                    Cfg.Save();
                }

                return (T)Cfg.Data[TName];
            }

            Var& operator=(T Data)
            {
                auto Cfg = Emulator::Get()->Cfg;
                Cfg.Data[TName] = Data;
                Cfg.Save();
                return *this;
            }
        private:
            T Default;
        };

        template<const char* TName> using String = Var<std::string, TName>;
        template<const char* TName> using Int = Var<I32, TName>;
        template<const char* TName> using Double = Var<double, TName>;
        template<const char* TName> using Bool = Var<bool, TName>;
    }
}