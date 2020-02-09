#include "rsx.h"

#include <spdlog/spdlog.h>

namespace volts::rsx
{
    std::vector<render*>& renders()
    {
        static std::vector<render*> it = {};
        return it;
    }

    void add(render* r)
    {
        renders().push_back(r);
        spdlog::info("connected renderer {}", r->name());
    }
}