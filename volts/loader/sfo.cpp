#include "sfo.h"

#include "svl/types.h"
#include "svl/stream.h"
#include "svl/convert.h"

#include <spdlog/spdlog.h>

using namespace std;
using namespace svl;
namespace cvt = convert;

namespace volts::loader::sfo
{
    struct index_table_entry
    {
        u16 key_offset;
        format data_format;
        u32 data_length;
        u32 max_length;
        u32 data_offset;
    };

    struct header
    {
        u32 magic;
        u32 version;
        u32 key_offset;
        u32 data_offset;
        u32 total_entries;
    };

    std::optional<object> load(svl::iostream& stream)
    {
        const auto head = svl::read<header>(stream);

        if(head.magic != cvt::to_u32("\0PSF"))
        {
            spdlog::error("invalid magic");
            return std::nullopt;
        }

        if(head.version != 0x101)
        {
            spdlog::error("unsupported version");
            return std::nullopt;
        }

        object val;

        for(int i = 0; i < head.total_entries; i++)
        {
            const auto redirect = svl::read<index_table_entry>(stream);
            const auto dist = stream.tell();

            stream.seek(head.key_offset + redirect.key_offset);

            string key;

            while(auto c = svl::read<char>(stream))
                key += c;

            stream.seek(head.data_offset + redirect.data_offset);

            const auto data = svl::read_n(stream, redirect.max_length);

            value v = { redirect.data_format, std::move(data) };

            val.insert({ key, v });

            stream.seek(dist);
        }

        return val;
    }
}