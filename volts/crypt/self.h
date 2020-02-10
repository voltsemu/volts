#pragma once

#include <types.h>
#include <file.h>

#include <vector>
#include <optional>

namespace volts::crypt::self
{
    /**
     * @brief decrypt and parse a self file
     * 
     * @param file the file to parse
     * @param key the decryption key to use if needed
     * @return svl::file the decrypted file, or empty
     */
    svl::file load(svl::file file, std::vector<svl::byte> key = {});
}