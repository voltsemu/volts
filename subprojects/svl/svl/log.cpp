#include <fmt/format.h>
#include <svl/log.h>
#include <svl/platform.h>

#include <memory>

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

namespace svl::log
{

    struct StdioSink : Sink
    {
        StdioSink()
        {
#if SYS_WINDOWS
            // UTF-8 console output for windows
            SetConsoleOutputCP(CP_UTF8);
            setvbuf(stdout, nullptr, _IOFBF, 1024);
#endif
        }

        virtual ~StdioSink() override {}

        virtual void debug(const std::string_view& str) override
        {
            fmt::print(KBLU "[debug]" KNRM " {}\n", str);
        }

        virtual void info(const std::string_view& str) override
        {
            fmt::print(KGRN "[info]" KNRM " {}\n", str);
        }

        virtual void warn(const std::string_view& str) override
        {
            fmt::print(KYEL "[warn]" KNRM " {}\n", str);
        }

        virtual void error(const std::string_view& str) override
        {
            fmt::print(KRED "[error]" KNRM " {}\n", str);
        }

        virtual void fatal(const std::string_view& str) override
        {
            fmt::print(KMAG "[fatal]" KNRM " {}\n", str);
        }
    };

    std::unique_ptr<Sink> sink;
    Level lvl;

    void set_level(Level l)
    {
        lvl = l;
    }

    Level level()
    {
        return lvl;
    }

    void init(Level l)
    {
        sink = std::unique_ptr<Sink>(new StdioSink());
        lvl = l;
    }

    void inner::debug(const std::string_view& str)
    {
        sink->debug(str);
    }

    void inner::info(const std::string_view& str)
    {
        sink->info(str);
    }

    void inner::warn(const std::string_view& str)
    {
        sink->warn(str);
    }

    void inner::error(const std::string_view& str)
    {
        sink->error(str);
    }

    void inner::fatal(const std::string_view& str)
    {
        sink->fatal(str);
    }
} // namespace svl::log