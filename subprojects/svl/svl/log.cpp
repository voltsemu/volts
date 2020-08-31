#include "log.h"

namespace svl::log {
    void stdlog::debug(std::string&& str) {
        fprintf(stream, "[error]: %s\n", str.c_str());
    }

    void stdlog::info(std::string&& str) {

    }

    void stdlog::warn(std::string&& str) {

    }

    void stdlog::err(std::string&& str) {

    }

    void stdlog::fatal(std::string&& str) {

    }

    unique<logger> stream = std::make_unique<stdlog>(stdout);
    level lvl = level::info;
}
