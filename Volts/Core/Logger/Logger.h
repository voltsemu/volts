#pragma once

#include <stdio.h>
#include <Core/Collections/CthulhuString.h>
#include "Volts/Config.h"

namespace Volts
{
    // how bad is the thing we logged?
    enum class Level
    {
        // a trace, possibly for a function call or single operating
        Trace,
        // for debugging, such as logging a struct to file for human reading
        Debug,
        // info the end user might care about, such as the path of the game executable, etc
        Info,
        // something bad has happened.
        // but not bad enough to warrant crashing or exiting.
        Warning,
        // something very bad has happened.
        // may have damaged emulator integrity
        // some stuff may stop functioning
        Error,
        // something so bad has happened the emulator has exited non gracefully
        // possibly internal emulator error or some very bad data was passed in
        Fatal,
    };

    // the current logging level, all messages below this will not be logged
    extern Level LogLevel;

    // print the associated prefix for a level
    void PrintPrefix(Level);

    enum class VideoDriver : Cthulhu::U8
    {
        DirectX9 = (1 << 0),
        DirectX11 = (1 << 1),
        DirectX12 = (1 << 2),
        OpenGL = (1 << 3),
        Vulkan = (1 << 4),
        OpenCL = (1 << 5),
        None = (1 << 6),
    };

    enum class GPUVendor : Cthulhu::U8
    {
        AMD, ///< AMD/ATI cards
        NVidia, ///< NVidia cards
        Intel, ///< Intel iGPUs
    };

    enum class CPUVendor : Cthulhu::U8
    {
        AMD,
        Intel,
    };

    enum class CPUDesign : Cthulhu::U8
    {
        Chiplet, ///< AMD ryzen cpus with more than 4 cores are made of chiplets, we need to know this so we can put threads on specific cores to reduce CXX lag
        Single, ///< Intel CPUs are just one big blob of cores, this reduces CXX latency
    };

    VideoDriver SupportedDrivers();

    Cthulhu::String GPUDriver();
    Cthulhu::String OSName();


// basic logging with formatting
#define LOGF(C, L, M, ...) { if(L >= LogLevel) { PrintPrefix(L); printf("[" #C "] " M "\n", __VA_ARGS__); } }

#define LOGF_TRACE(C, M, ...) LOGF(C, Level::Trace, M, __VA_ARGS__)
#define LOGF_INFO(C, M, ...) LOGF(C, Level::Info, M, __VA_ARGS__)

#if VDEBUG
#   define LOGF_DEBUG(C, M, ...) { PrintPrefix(Level::Debug); printf("[" #C "] " M "\n", __VA_ARGS__); }
#else
#   define LOGF_DEBUG(C, M, ...)
#endif

#define LOGF_WARNING(C, M, ...) LOGF(C, Level::Warning, M, __VA_ARGS__)
#define LOGF_ERROR(C, M, ...) LOGF(C, Level::Error, M, __VA_ARGS__)
#define LOGF_FATAL(C, M, ...) LOGF(C, Level::Fatal, M, __VA_ARGS__)

// basic logging without formatting
#define LOG(C, L, M) { if(L >= LogLevel) { PrintPrefix(L); printf("[" #C "] " M "\n"); } }

#define LOG_TRACE(C, M) LOG(C, Level::Trace, M)
#define LOG_INFO(C, M) LOG(C, Level::Info, M)

#if VDEBUG
#   define LOG_DEBUG(C, M) { PrintPrefix(Level::Debug); printf("[" #C "] " M "\n"); }
#else
#   define LOG_DEBUG(C, M)
#endif

#define LOG_WARNING(C, M) LOG(C, Level::Warning, M)
#define LOG_ERROR(C, M) LOG(C, Level::Error, M)
#define LOG_FATAL(C, M) LOG(C, Level::Fatal, M)
}