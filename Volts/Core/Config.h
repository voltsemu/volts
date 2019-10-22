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

        fs::path ConfigPath{};
        fs::path ConfigDir{};
        
        argo::json Data;
    };
}