#pragma once

extern "C" {
    // cant include <Meta/Macros.h> because visual studio needs to include this
#if defined(_WIN32) || defined(_WIN64)
    __declspec(dllexport)
#endif
    // ugly void* to stop msvc spazzing out
    int VoltsMain(void* Handle); 
}