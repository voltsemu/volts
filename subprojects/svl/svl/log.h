#pragma once

#include <fmt/format.h>
#include <string_view>

namespace svl::log
{
    struct Sink
    {
        virtual ~Sink() {}
        virtual void debug(const std::string_view& str) = 0;
        virtual void info(const std::string_view& str) = 0;
        virtual void warn(const std::string_view& str) = 0;
        virtual void error(const std::string_view& str) = 0;
        virtual void fatal(const std::string_view& str) = 0;
    };

    enum class Level
    {
        debug = 0,
        info = 1,
        warn = 2,
        error = 3,
        fatal = 4
    };

    namespace inner
    {
        void debug(const std::string_view& str);
        void info(const std::string_view& str);
        void warn(const std::string_view& str);
        void error(const std::string_view& str);
        void fatal(const std::string_view& str);
    } // namespace inner

    Level level();
    void set_level(Level lvl);
    void init(Level lvl = Level::info);

#define LOG_MACRO(NAME)                                     \
    template <typename... T>                                \
    void NAME(const std::string_view& str, T&&... args)    \
    {                                                       \
        if (level() <= Level::NAME)                         \
        {                                                   \
            inner::NAME(fmt::format(str, args...));         \
        }                                                   \
    }

    LOG_MACRO(debug)
    LOG_MACRO(info)
    LOG_MACRO(warn)
    LOG_MACRO(error)

    template <typename... T>
    [[noreturn]] void fatal(const std::string_view& str, T&&... args)
    {
        inner::fatal(fmt::format(str, args...));
        std::abort();
    }
} // namespace svl::log

#define EXPECT(expr)    \
  if (!(expr)) { svl::log::fatal("[E0007]" STR(__FILE__) STR(__FILE__) "Assertion failed" STR(expr)); }
