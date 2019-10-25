#pragma once

#include <filesystem>

#include <argo.hpp>

#include <machineid/machineid.h>

namespace Volts
{
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
}