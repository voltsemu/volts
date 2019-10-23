#pragma once

#include <filesystem>

#include <argo.hpp>

namespace Volts
{
    namespace fs = std::filesystem;

    struct Config
    {
        Config();

        void Init();

        void Reload();
        void Save();

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

        fs::path ConfigPath{};
        fs::path ConfigDir{};
        
        argo::json Data;
    };
}