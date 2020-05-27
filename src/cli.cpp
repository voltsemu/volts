#include <fstream>

#include <svl/traits.h>
#include <svl/convert.h>

#define CXXOPTS_NO_EXCEPTIONS
#define CXXOPTS_NO_RTTI
#include <cxxopts.hpp>

#include <toml++/toml.h>

#include "volts/loader/sfo.h"


namespace volts::cli
{
    namespace opts = cxxopts;
    namespace cvt = svl::convert;

    struct Options
    {
        using Handler = std::function<void(const opts::OptionValue&)>;

        Options(const std::string& name, const std::string& desc)
            : parser(name, desc)
        { }

        template<typename T = void>
        void add(const std::string& arg, const std::string& desc, Handler&& func)
        {
            if constexpr (std::is_void<T>::value)
            {
                // TODO: please tell me this isnt how its meant to work
                parser.add_options()(arg, desc);
            }
            else
            {
                parser.add_options()(arg, desc, opts::value<T>());
            }

            options.insert({ arg, func });
        }

        void parse(int argc, char** argv)
        {
            auto res = parser.parse(argc, argv);

            bool matched = false;

            for (const auto& [arg, func] : options)
            {
                if (res.count(arg))
                {
                    matched = true;
                    func(res[arg]);
                }
            }

            if (!matched)
            {
                fmt::print(parser.help());
            }
        }

        std::unordered_map<std::string, Handler> options;
        opts::Options parser;
    };
}

volts::sfo::Value make_value(toml::node&& node)
{
    if (node.is_string())
    {
        return std::move(node.as_string()->get());
    }
    else if (node.is_integer())
    {
        return (svl::i32)node.as_integer()->get();
    }
    else if (node.is_array())
    {
        std::vector<svl::byte> bytes;

        for (const auto& item : *node.as_array())
        {
            ASSERT(item.is_integer());
            bytes.push_back((svl::byte)item.as_integer()->get());
        }

        return std::move(bytes);
    }
    else
    {
        svl::panic("[E0002] Invalid SFO data type");
    }
}

int main(int argc, char** argv)
{
#if SYS_WINDOWS
    // UTF-8 console output for windows
    SetConsoleOutputCP(CP_UTF8);
    setvbuf(stdout, nullptr, _IOFBF, 1024);
#endif

    volts::cli::Options opts("volts", "ps3 command line tools");

    opts.add("help", "print help", [&](const auto&) {
        fmt::print(opts.parser.help());
        std::exit(0);
    });

    opts.add<std::string>("log", "set log level", [](const auto& res) {
        (void)res;
    });

    opts.add<std::string>("err", "lookup error by id", [](const auto& res) {
        (void)res;
    });

    opts.add<std::string>("sfo-read", "parse sfo to toml", [](const auto& res) {
        const fs::path path = res.as<std::string>();
        auto sfo = volts::sfo::load(std::move(svl::open(path)))
            .expect("failed to load sfo from {}", path);

        for (const auto& [key, val] : sfo)
        {
            std::string data = std::visit(svl::Visitor {
                [](const std::string& str) { return fmt::format("\"{}\"", str); },
                [](const std::vector<svl::byte>& vec) { return svl::convert::to_string(vec); },
                [](svl::i32 i) { return std::to_string(i); }
            }, val);

            fmt::print("{} = {}\n", key, data);
        }
    });

    opts.add<std::string>("sfo-write", "parse toml to sfo", [](const auto& res) {
        const fs::path path = res.as<std::string>();
        auto data = toml::parse_file(path.string());

        volts::sfo::Object obj;

        for (const auto& [key, val] : data)
        {
            obj[key] = make_value(std::move(val));
        }

        volts::sfo::unload(obj)
            .save("PARAM.SFO");
    });

    opts.parse(argc, argv);
}