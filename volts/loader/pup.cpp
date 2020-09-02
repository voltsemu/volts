#include "pup.h"

#include <svl/cvt.h>

namespace vt::pup {
    struct header {
        u64 magic;
        big<u64> pkg_version;
        big<u64> img_version;
        big<u64> files;
        big<u64> hdr_size;
        big<u64> data_size;
    };

    svl::file object::get(u64 id) {
        for (auto& file : files) {
            if (file.id == id) {
                source.seek(file.addr);
                return svl::from(source.read<u8>(file.len));
            }
        }

        return svl::buffer();
    }

    std::optional<object> load(svl::file&& source) {
        auto head = source.read<header>();

        if (head.magic != cvt::u64("SCEUF\0\0\0")) {
            return std::nullopt;
        }

        auto files = source.read<entry>(head.files);
        auto hashes = source.read<hash>(head.files);

        auto out = object(std::move(source));
        out.files = files;
        out.hashes = hashes;

        return out;
    }
}