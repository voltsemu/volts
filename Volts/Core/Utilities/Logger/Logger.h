#pragma once

#include <Core/Collections/Array.h>
#include <Core/Collections/CthulhuString.h>

namespace Volts
{
    using Str = Cthulhu::String;

    enum class Level 
    {
        Trace,
        Info,
        Debug,
        Warning,
        Error,
        Fatal,
    };

    extern Level LogLevel;

    void PrintPrefix(Level);

#define LOGF(C, L, M, ...) { if(L >= LogLevel) { PrintPrefix(L); printf("[%s] ", C); printf(M "\n", __VA_ARGS__); } }

#define LOGF_TRACE(C, M, ...) LOGF(C, Level::Trace, M, __VA_ARGS__)
#define LOGF_INFO(C, M, ...) LOGF(C, Level::Info, M, __VA_ARGS__)
#define LOGF_DEBUG(C, M, ...) LOGF(C, Level::Debug, M, __VA_ARGS__)
#define LOGF_WARNING(C, M, ...) LOGF(C, Level::Warning, M, __VA_ARGS__)
#define LOGF_ERROR(C, M, ...) LOGF(C, Level::Error, M, __VA_ARGS__)
#define LOGF_FATAL(C, M, ...) LOGF(C, Level::Fatal, M, __VA_ARGS__)

#define LOG(C, L, M) { if(L >= LogLevel) { PrintPrefix(L); printf("[%s] ", C); printf(M "\n"); } }

#define LOG_TRACE(C, M) LOG(C, Level::Trace, M)
#define LOG_INFO(C, M) LOG(C, Level::Info, M)
#define LOG_DEBUG(C, M) LOG(C, Level::Debug, M)
#define LOG_WARNING(C, M) LOG(C, Level::Warning, M)
#define LOG_ERROR(C, M) LOG(C, Level::Error, M)
#define LOG_FATAL(C, M) LOG(C, Level::Fatal, M)
}