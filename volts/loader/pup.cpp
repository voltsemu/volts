#include "pup.h"

#include <spdlog/spdlog.h>

#include "svl/stream.h"
#include "svl/convert.h"

using namespace svl;
using namespace endian;

namespace cvt = svl::convert;

namespace volts::loader::pup
{
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

    std::vector<svl::byte> object::get_file(svl::u64 id)
    {
        auto f = std::find_if(std::begin(files), std::end(files), [id](auto& file) {
            return file.id == id;
        });

        if(f == std::end(files))
            return {};

        file->seek(f->offset.get());
        return read_n(*file, f->length);
    }

    std::optional<object> load(std::shared_ptr<svl::iostream> stream)
    {
        auto head = read<header>(*stream);

        if(head.magic != cvt::to_u64("SCEUF\0\0\0"))
        {
            spdlog::error("invalid pup magic");
            return std::nullopt;
        }

        object ret = stream;

        ret.files = read_n<entry>(*stream, head.file_count);
        ret.hashes = read_n<hash>(*stream, head.file_count);

        return ret;
    }
}