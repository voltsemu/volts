#pragma once

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

// basic logging with formatting
#define LOGF(C, L, M, ...) { if(L >= LogLevel) { PrintPrefix(L); printf("[" #C "] " M "\n", __VA_ARGS__); } }

#define LOGF_TRACE(C, M, ...) LOGF(C, Level::Trace, M, __VA_ARGS__)
#define LOGF_INFO(C, M, ...) LOGF(C, Level::Info, M, __VA_ARGS__)

#if VDEBUG
#   define LOGF_DEBUG(C, M, ...) LOGF(C, Level::Debug, M, __VA_ARGS__)
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
#   define LOG_DEBUG(C, M) LOG(C, Level::Debug, M)
#else
#   define LOG_DEBUG(C, M)
#endif

#define LOG_WARNING(C, M) LOG(C, Level::Warning, M)
#define LOG_ERROR(C, M) LOG(C, Level::Error, M)
#define LOG_FATAL(C, M) LOG(C, Level::Fatal, M)
}