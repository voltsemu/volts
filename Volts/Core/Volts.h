#pragma once

// put in an extern block for compatibilitys sake
extern "C" { 

// cant include <Meta/Macros.h> because visual studio needs to include this
#if defined(_WIN32) || defined(_WIN64)
    __declspec(dllexport)
#endif
    int VoltsMain(); 
}