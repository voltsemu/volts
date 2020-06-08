#include <fmt/core.h>

#include <svl/logger.h>

#include "volts/loader/self.h"

int main(int argc, char **argv)
{
    (void)argc;
    svl::log::init();

    auto file = svl::open(argv[1]);
    auto data = volts::self::load(std::move(file));
    return 0;
}