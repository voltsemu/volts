#pragma once

#include "svl/platform.h"

#include <memory>
#include <fmt/core.h>

namespace svl
{
    struct Logger
    {
        virtual ~Logger() { }
        virtual void debug(const char *str) = 0;
        virtual void info(const char *str) = 0;
        virtual void warn(const char *str) = 0;
        virtual void error(const char *str) = 0;
        virtual void fatal(const char *str) = 0;

        static void set(Logger *logger);
        static Logger *get();
    };

    namespace log
    {
        enum class Level
        {
            debug = 0,
            info = 1,
            warn = 2,
            error = 3,
            fatal = 4
        };

        extern Level level;

        void init(Level lvl = Level::info);
        void deinit();
    }

    template<typename... T>
    void debug(const char *fmt, T&&... args)
    {
        if (log::Level::debug >= log::level)
            Logger::get()->debug(fmt::format(fmt, args...).c_str());
    }

    template<typename... T>
    void info(const char *fmt, T&&... args)
    {
        if (log::Level::info >= log::level)
            Logger::get()->info(fmt::format(fmt, args...).c_str());
    }

    template<typename... T>
    void warn(const char *fmt, T&&... args)
    {
        if (log::Level::warn >= log::level)
            Logger::get()->warn(fmt::format(fmt, args...).c_str());
    }

    template<typename... T>
    void error(const char *fmt, T&&... args)
    {
        if (log::Level::error >= log::level)
            Logger::get()->error(fmt::format(fmt, args...).c_str());
    }

    template<typename... T>
    [[noreturn]]
    void fatal(const char *fmt, T&&... args)
    {
        Logger::get()->fatal(fmt::format(fmt, args...).c_str());
        std::abort();
    }
}