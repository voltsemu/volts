#pragma once

#include <svl/file.h>
#include <vector>

namespace vt::sce {
    std::vector<svl::file> load(svl::file&& source);
}
