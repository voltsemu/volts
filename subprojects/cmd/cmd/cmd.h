#pragma once

#include <svl/types.h>

#include <functional>
#include <string>
#include <map>
#include <variant>
#include <initializer_list>

namespace cmd
{
    struct parser;

    using handler = std::function<void(parser*)>;
    using callback = std::function<void(std::string&&)>;

    struct flag
    {
        flag(const char* iname, const char* idesc)
            : name(iname)
            , desc(idesc)
        { }

        flag* operator+=(handler ifunc)
        {
            func = ifunc;
            return this;
        }

        handler func;
        const char* name;
        const char* desc;
    };

    struct arg
    {
        arg(const char* iname, const char* idesc)
            : name(iname)
            , desc(idesc)
        { }

        arg* operator+=(callback ifunc)
        {
            func = ifunc;
            return this;
        }

        callback func;
        const char* name;
        const char* desc;
    };

    using argument = std::variant<flag*, arg*>;

    struct parser
    {
        parser(int iargc, char** iargv)
            : argc(iargc)
            , argv(iargv)
        { }

        parser* with(std::initializer_list<argument> iargs)
        {
            for (auto iarg : iargs)
            {
                std::visit(svl::visitor {
                    [&](flag* flag) { flags[flag->name] = flag; },
                    [&](arg* arg) { args[arg->name] = arg; }
                }, iarg);
            }

            for (int i = 0; i < argc; i++)
            {
                if (auto flag = flags.find(argv[argc]); flag != flags.end())
                {
                    flag->second->func(this);
                }
                else if (auto arg = args.find(argv[argc]); arg != args.end())
                {
                    arg->second->func(argv[argc+1]);
                }
            }

            return this;
        }

        std::string help()
        {
            return "";
        }

    private:
        std::map<std::string, arg*> args;
        std::map<std::string, flag*> flags;
        int argc;
        char** argv;
    };
}
