#include "sfo.h"

#include "svl/types.h"
#include "svl/file.h"
#include "svl/convert.h"

#include <spdlog/spdlog.h>

namespace volts::loader::sfo
{
    using namespace std;
    using namespace svl;
    namespace cvt = convert;

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

    std::optional<object> load(svl::file stream)
    {
        const auto head = stream.read<header>();

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
            const auto redirect = stream.read<index_table_entry>();
            const auto dist = stream.tell();

            stream.seek(head.key_offset + redirect.key_offset);

            string key;

            while(auto c = stream.read<char>())
                key += c;

            stream.seek(head.data_offset + redirect.data_offset);

            const auto data = stream.read<svl::byte>(redirect.max_length);

            value v = { redirect.data_format, std::move(data) };

            val.insert({ key, v });

            stream.seek(dist);
        }

        return val;
    }
}