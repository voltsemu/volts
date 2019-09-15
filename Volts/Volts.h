#include "Core/Collections/CthulhuString.h"
#include "Core/Macros.h"
#include "Core/Logger/Logger.h"

#pragma once

#if OS_WINDOWS
#   define VEXTERN_API __declspec(dllexport)
#   define VDECL __cdecl
#else
#   define VEXTERN_API
#   define VDECL
#endif

typedef void(*LogFunc)(const char*, int);

// load the emulator stuff up so we can play games
extern "C" VEXTERN_API void VDECL Load(LogFunc);

// unload everything to shut down cleanly
extern "C" VEXTERN_API void VDECL Unload();

namespace Volts
{

}