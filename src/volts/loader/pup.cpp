#include "pup.h"

#include <svl/convert.h>

namespace volts::loader::pup
{
    using namespace svl;

    namespace cvt = svl::convert;

    struct Header
    {
        u64 magic;
        endian::Big<u64> package_version;
        endian::Big<u64> image_version;
        endian::Big<u64> file_count;
        endian::Big<u64> header_length;
        endian::Big<u64> data_length;
    };

    svl::Option<svl::File> Object::get(u64 id)
    {
        for (const auto& file : files)
        {
            if (file.id == id)
            {
                source.seek(file.offset);
                return svl::stream(source.read(file.length));
            }
        }

        return svl::none();
    }

    svl::Option<Object> load(svl::File&& source)
    {
        const auto header = source.read<Header>();

        if (header.magic != cvt::to_u64("SCEUF\0\0\0"))
        {
            log::error("invalid pup magic");
            return none();
        }

        auto files = source.read<Entry>(header.file_count);
        auto hashes = source.read<Hash>(header.file_count);

        return {{files, hashes, std::move(source)}};
    }
} // namespace volts::loader::pup