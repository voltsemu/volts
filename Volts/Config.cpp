#include "Config.h"

#include "Core/Logger/Logger.h"

#include <string.h>
#include <stdio.h>

#if OS_WINDOWS
#   include <windows.h>
#elif OS_APPLE
#   include <mach-o/dyld.h>
#else
#   include <unistd.h>
#endif

#include <rapidjson/document.h>

namespace Volts::Config
{
    using namespace Cthulhu;
    using namespace rapidjson;

    Document ConfigData;

    std::string ConfigPath()
    {
        char Path[1024] = {};
#if OS_WINDOWS
        GetModuleFileName(nullptr, Path, sizeof(Path));
#elif OS_APPLE
        U32 _;
        _NSGetExecutablePath(Path, &_);
#else
        readlink("/proc/self/exe", Path, sizeof(Path));
#endif

        return fmt::format("{}/Data/config.json", Path);
    }

    const char* CreateFile()
    {
        
    }
    
    const char* OpenFile()
    {

    }

    void UpdateFile()
    {
        
    }

    void Load()
    {
        /* std::string Name = ConfigPath();
        puts(Name.c_str());
        FILE* Json = fopen(Name.c_str(), "rb");
        if(!Json)
        {
            Json = fopen(Name.c_str(), "wb");
        }
        fseek(Json, 0, SEEK_END);
        U32 Size = ftell(Json);
        fseek(Json, 0, SEEK_SET);

        char* Data = new char[Size + 1];
        fread(Data, Size, 1, Json);
        fclose(Json);

        Data[Size] = '\0';

        if(ConfigData.Parse(Data).HasParseError())
        {
            VERROR("Config file failed to parse, generating new file");
        }
        else
        {
            VINFO("Config filed parsed");
        }

        delete[] Data; */
    }

    const char* CurrentRender()
    {
        return "Metal";// ConfigData["CurrentRender"].GetString();
    }
}