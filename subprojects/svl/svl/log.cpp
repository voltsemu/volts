#include "log.h"

// https://github.com/gon1332/fort320/blob/master/include/Utils/colors.h
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


namespace svl::log {
    void stdlog::debug(std::string&& str) {
        fprintf(stream, FCYN("[debug]:") " %s\n", str.c_str());
    }

    void stdlog::info(std::string&& str) {
        fprintf(stream, FBLU("[info]:") " %s\n", str.c_str());
    }

    void stdlog::warn(std::string&& str) {
        fprintf(stream, FYEL("[info]:") " %s\n", str.c_str());
    }

    void stdlog::err(std::string&& str) {
        fprintf(stream, FRED("[info]:") " %s\n", str.c_str());
    }

    void stdlog::fatal(std::string&& str) {
        fprintf(stream, FMAG("[info]:") " %s\n", str.c_str());
    }

    void flog::debug(std::string&& str) {
        handle.write(str.data(), str.size());
        handle.write('\n');
    }

    void flog::info(std::string&& str) {
        handle.write(str.data(), str.size());
        handle.write('\n');
    }

    void flog::warn(std::string&& str) {
        handle.write(str.data(), str.size());
        handle.write('\n');
    }

    void flog::err(std::string&& str) {
        handle.write(str.data(), str.size());
        handle.write('\n');
    }

    void flog::fatal(std::string&& str) {
        handle.write(str.data(), str.size());
        handle.write('\n');
    }

    unique<logger> stream = std::make_unique<stdlog>(stdout);
    level lvl = level::info;
}
