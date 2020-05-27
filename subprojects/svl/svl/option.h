#pragma once

#include <svl/panic.h>

#include <optional>

namespace svl
{
    struct Nothing {};

    template<typename T>
    struct Option
    {
        Option(Nothing) : data(std::nullopt) { }
        Option(T&& val) : data(std::move(val)) { }

        template<typename... TArgs>
        T expect(const std::string& msg, TArgs&&... args)
        {
            if (!data)
            {
                svl::panic(msg, args...);
            }

            return data.value();
        }

    private:
        std::optional<T> data;
    };

    inline Nothing none()
    {
        return Nothing{};
    }
}