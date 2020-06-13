#include <fmt/core.h>

#include <svl/logger.h>
#include <crypt/aes.h>

#include "volts/loader/self.h"


int main(int argc, char **argv)
{
    (void)argc;
    svl::log::init();
    crypt::init();

    auto file = svl::open(argv[1]);
    auto data = volts::self::load(std::move(file));
    svl::log::deinit();
    return 0;
}