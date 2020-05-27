#pragma once

#include <optional>

namespace svl
{
    struct Nothing {};

    template<typename T>
    struct Option
    {
        Option(Nothing) : data(std::nullopt) { }
        Option(T&& val) : data(std::move(val)) { }

    private:
        std::optional<T> data;
    };

    Nothing none()
    {
        return Nothing{};
    }
}