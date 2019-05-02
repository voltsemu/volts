#pragma once

// cant include <Meta/Macros.h> because visual studio needs to include this
#if defined(_WIN32) || defined(_WIN64)
#   define VOLTS_API __declspec(dllexport)
#else
#   define VOLTS_API
#endif

extern "C" {
    // ugly void* to stop msvc spazzing out
    VOLTS_API int VoltsMain(void* Handle); 

    // the return types from these need a static_cast to be useful
    // but as external shit has to include this this file cant have #includes
    VOLTS_API void SetLogLevel(short);
    VOLTS_API short GetLogLevel();
}

namespace Volts 
{
    extern struct Native* Window;
}