#pragma once

#include <fmt/core.h>
#include <string_view>
#include "types.h"

namespace svl::log {
    enum class level {
        debug = 0,
        info = 1,
        warn = 2,
        err = 3,
        fatal = 4
    };

    struct logger {
        virtual void debug(std::string&& str) = 0;
        virtual void info(std::string&& str) = 0;
        virtual void warn(std::string&& str) = 0;
        virtual void err(std::string&& str) = 0;
        virtual void fatal(std::string&& str) = 0;
    };

    struct stdlog : logger {
        stdlog(FILE* s) : stream(s) { }
        virtual void debug(std::string&& str) override;
        virtual void info(std::string&& str) override;
        virtual void warn(std::string&& str) override;
        virtual void err(std::string&& str) override;
        virtual void fatal(std::string&& str) override;

    private:
        FILE* stream;
    };

    extern unique<logger> stream;
    extern level lvl;

#define LOG_FUNC(name) \
    template<typename... T> \
    void name(std::string_view fmt, T&&... args) { \
        if (lvl <= level::name) \
            stream->name(fmt::format(fmt, args)); \
    }

    LOG_FUNC(debug)
    LOG_FUNC(info)
    LOG_FUNC(warn)
    LOG_FUNC(err)
    LOG_FUNC(fatal)
}