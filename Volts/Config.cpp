#include "Config.h"

#include "Core/Logger/Logger.h"

#include <string.h>
#include <stdio.h>

#if OS_WINDOWS
#   include <windows.h>
#else
#   include <unistd.h>
#endif

#include <rapidjson/document.h>

namespace Volts::Config
{
    using namespace Cthulhu;
    using namespace rapidjson;

    Document ConfigData;
    void Load()
    {
        char Path[1024] = {};
#if OS_WINDOWS
        GetModuleFileName(nullptr, Path, sizeof(Path));
#else
        readlink("/proc/self/exe", Path, sizeof(Path));
#endif

        FILE* Json = fopen(fmt::format("{}/config.json", Path).c_str(), "rb");
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

        delete[] Data;
    }

    const char* CurrentRender()
    {
        return ConfigData["CurrentRender"].GetString();
    }
}