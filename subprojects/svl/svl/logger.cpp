#include "svl/logger.h"

// https://github.com/gon1332/fort320/blob/master/include/Utils/colors.h
/* FOREGROUND */
#define RST  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define FRED(x) KRED x RST
#define FGRN(x) KGRN x RST
#define FYEL(x) KYEL x RST
#define FBLU(x) KBLU x RST
#define FMAG(x) KMAG x RST
#define FCYN(x) KCYN x RST
#define FWHT(x) KWHT x RST

#define BOLD(x) "\x1B[1m" x RST
#define UNDL(x) "\x1B[4m" x RST

namespace svl
{
    struct StdioLogger : Logger
    {
        virtual ~StdioLogger() override { }
        virtual void debug(const char *str) override
        {
            printf(FBLU("[debug]") " %s\n", str);
        }

        virtual void info(const char *str) override
        {
            printf(FGRN("[info]") " %s\n", str);
        }

        virtual void warn(const char *str) override
        {
            printf(FYEL("[warn]") " %s\n", str);
        }

        virtual void error(const char *str) override
        {
            printf(FRED("[error]") " %s\n", str);
        }

        virtual void fatal(const char *str) override
        {
            printf(FCYN("[fatal]") " %s\n", str);
        }
    };

    Logger *logger = nullptr;

    void init()
    {
        logger = new StdioLogger();
    }

    void Logger::set(Logger *in)
    {
        logger = in;
    }

    Logger *Logger::get()
    {
        return logger;
    }
}