#include <fmt/core.h>

#include <svl/logger.h>

#include "volts/loader/self.h"
#include "smallcrypt/aes.h"


int main(int argc, char **argv)
{
    (void)argc;
    svl::log::init();
    smallcrypt::init();

    auto file = svl::open(argv[1]);
    auto data = volts::self::load(std::move(file));
    svl::log::deinit();
    return 0;
}