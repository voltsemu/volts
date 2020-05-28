#pragma once

#include <string>
#include <unordered_map>
#include <functional>

namespace cmd
{
    using callback = std::function<void(const std::string&)>;

    struct arg
    {
        std::string_view desc;
        callback func;
    };

    struct options
    {
        options(const std::string_view& n, const std::string_view& d)
            : name(n)
            , desc(d)
        { }

        void add(const std::string_view& name, const std::string_view& desc, callback func)
        {
            callbacks[name] = { desc, func };
        }

        std::string help()
        {
            
        }

    private:
        const std::string_view name;
        const std::string_view desc;
        std::unordered_map<std::string_view, arg> callbacks;
    };
}