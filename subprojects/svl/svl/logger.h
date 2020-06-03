#pragma once

#include "svl/platform.h"

#include <memory>
#include <fmt/core.h>

namespace svl
{
    struct Logger
    {
        virtual ~Logger() { }
        virtual void debug(const char* str) = 0;
        virtual void info(const char* str) = 0;
        virtual void warn(const char* str) = 0;
        virtual void error(const char* str) = 0;
        virtual void fatal(const char* str) = 0;

        static void set(Logger* logger);
        static Logger* get();
    };

    template<typename... T>
    void debug(const char* fmt, T&&... args)
    {
        Logger::get()->debug(fmt::format(fmt, args...).c_str());
    }

    template<typename... T>
    void info(const char* fmt, T&&... args)
    {
        Logger::get()->info(fmt::format(fmt, args...).c_str());
    }

    template<typename... T>
    void warn(const char* fmt, T&&... args)
    {
        Logger::get()->warn(fmt::format(fmt, args...).c_str());
    }

    template<typename... T>
    void error(const char* fmt, T&&... args)
    {
        Logger::get()->error(fmt::format(fmt, args...).c_str());
    }

    template<typename... T>
    [[noreturn]]
    void fatal(const char* fmt, T&&... args)
    {
        Logger::get()->fatal(fmt::format(fmt, args...).c_str());
        std::abort();
    }
}