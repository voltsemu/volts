#pragma once

#include "svl/types.h"
#include "svl/file.h"

#include <vector>

namespace volts::loader::unself
{
    /**
     * @brief parse an sce file
     * 
     * @param file the file to parse
     * @return std::vector<svl::file> an array of every file contained inside the file 
     */
    std::vector<svl::file> load_sce(svl::file file);
}