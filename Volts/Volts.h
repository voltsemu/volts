#include "Core/Collections/CthulhuString.h"
#include "Core/Macros.h"
#include "Core/Logger/Logger.h"

#pragma once

#if OS_WINDOWS
#   define VEXTERN_API __declspec(dllexport)
#else
#   define VEXTERN_API
#endif

extern "C" 
{
    // load the emulator stuff up so we can play games
    void VEXTERN_API Load();
    // unload everything to shut down cleanly
    void VEXTERN_API Unload();
}

namespace Volts
{

}