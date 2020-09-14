#include "sce.h"

#include "common.h"

#include <svl/cvt.h>

namespace vt::sce {
    using namespace svl;

    std::vector<svl::file> load(svl::file&& source) {
        auto head = source.read<sce::header>();

        if (head.magic != cvt::u32("SCE\0")) {
            return {};
        }

        auto metadata_size 
            = head.end - sizeof(sce::header)
            + head.start + sizeof(metadata::info);

        source.seek(head.start + sizeof(sce::header));
        auto meta_info = source.read<u8>(sizeof(metadata::info));
        auto meta_head = source.read<u8>(metadata_size);

        u8 key[32];
        u8 iv[16];
    }
}