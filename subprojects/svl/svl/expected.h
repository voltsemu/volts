#pragma once

#include <string>

#include <spdlog/spdlog.h>

namespace svl
{
    using nothing = struct {};

    constexpr nothing none() { return nothing(); }

    template<typename T>
    struct expected
    {
        expected(nothing)
            : has_value(false)
        {}

        expected(T val)
            : has_value(true)
            , data(val)
        {}

        T expect(const std::string& message) const
        {
            if(has_value)
                return data;

            spdlog::critical(message);
            std::abort();
        }

        T value() const
        {
            if(has_value)
                return data;

            spdlog::critical("value() on empty");
            std::abort();
        }

        T value_or(T val) const { return has_value ? data : val; }

        template<typename F>
        expected<T> map(F&& func) const { return has_value ? func(data) : none(); }

        operator bool() const { return has_value; }
        bool valid() const { return has_value; }
    private:
        bool has_value;
        T data;
    };
}