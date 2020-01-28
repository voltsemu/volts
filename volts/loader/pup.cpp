#include "pup.h"

#include <spdlog/spdlog.h>

#include "svl/file.h"
#include "svl/convert.h"

namespace volts::loader::pup
{
    using namespace svl;
    using namespace endian;

    namespace cvt = svl::convert;

    struct header
    {
        u64 magic;
        big<u64> package_version;
        big<u64> image_version;
        big<u64> file_count;
        big<u64> header_length;
        big<u64> data_length;
    };

    static_assert(sizeof(header) == 48);

    svl::file object::get_file(svl::u64 id)
    {
        for(const auto& f : files)
        {
            if(f.id == id)
            {
                file.seek(f.offset);
                return svl::from(file.read<u8>(f.length));
            }
        }

        return {};
    }

    std::optional<object> load(svl::file stream)
    {
        auto head = stream.read<header>();

        if(head.magic != cvt::to_u64("SCEUF\0\0\0"))
        {
            spdlog::error("invalid pup magic");
            return std::nullopt;
        }

        object ret = { stream };

        ret.files = stream.read<entry>(head.file_count);
        ret.hashes = stream.read<hash>(head.file_count);

        return ret;
    }
}