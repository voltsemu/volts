#include "volts/loader/self.h"
#include "volts/loader/common.h"

#include <svl/convert.h>
#include <svl/logger.h>

namespace volts::self
{
    namespace cvt = svl::convert;

    svl::File load(svl::File &&source)
    {
        source.seek(0);
        auto sce_header = source.read<sce::Header>();

        if (sce_header.magic != cvt::u32("SCE\0"))
            svl::fatal("invalid SCE magic");

        return svl::stream();
    }
}