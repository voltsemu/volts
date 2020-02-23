#pragma once

#include <string>

#include <spdlog/spdlog.h>

namespace svl
{
    namespace detail
    {
        template<typename T> struct ok { ok(T v) : val(v) {} T val; operator T() const { return val; } };
        template<typename T> struct err { err(T v) : val(v) {} T val; operator T() const { return val; } };
    }

    template<typename T>
    detail::ok<T> ok(T val) { return detail::ok(val); }

    template<typename T>
    detail::err<T> err(T err) { return detail::err(err); }

    template<typename T, typename E>
    struct result
    {
        using self = result<T, E>;
        using error_type = E;
        using value_type = T;

        result(detail::ok<T> val)
            : success(true)
            , val(val)
        {}

        result(detail::err<E> err)
            : success(false)
            , err(err)
        {}

        ~result()
        {
            if(success)
                val.~T();
            else
                err.~E();
        }

        bool valid() const { return success; }
        operator bool() const { return success; }

        template<typename F>
        T expect(const std::string& message, F&& func) const
        {
            if(success)
                return val;

            spdlog::critical("{} {}", message, func(err));
            std::abort();
        }

        T expect(const std::string& message) const
        {
            if(success)
                return val;

            spdlog::critical(message);
            std::abort();
        }

        template<typename F>
        T unwrap(F&& func = [](E err) -> std::string {}) const
        {
            if(success)
                return val;

            spdlog::critical(func(err));
            std::abort();
        }

        template<typename F>
        T unwrap_or(F&& func = [](E err) -> T {}) const
        {
            return success ? val : func(err);
        }

        template<typename F>
        self map(F&& func) const
        {
            if(success)
                return ok(func(val));

            return err(err);
        }

        T value() const 
        {
            if(success)
                return val;

            spdlog::critical("value() on error");
            std::abort();
        }

        E error() const
        {
            if(!success)
                return err;

            spdlog::critical("error() on value");
            std::abort();
        }

    private:
        bool success;
        union 
        {
            T val;
            E err;
        };
    };
}