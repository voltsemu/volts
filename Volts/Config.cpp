#include "Config.h"

#include "Core/Logger/Logger.h"

#include <string.h>
#include <stdio.h>

namespace Volts::Config
{
    
    bool Load()
    {
        char* Begin = strdup(R"(
            {
                "i": 0,
                "d": "aaaa"
            }
        )");
        char* End = Begin + strlen(Begin);
        JsonValue val;
        JsonAllocator alloc;
        if(jsonParse(Begin, &End, &val, alloc) == JSON_OK) 
        {
            VINFO("Parsed config file")
            return true;
        }
        else
        {
            VINFO("Failed to parse config file")
            return false;
        }
    }
}