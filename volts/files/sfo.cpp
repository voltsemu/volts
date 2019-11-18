#include "sfo.h"

#include "svl/stream.h"
#include "svl/convert.h"

#include <spdlog/spdlog.h>

using namespace std;
namespace cvt = svl::convert;

namespace volts::files::sfo
{
    struct index_table_entry
    {
        std::uint16_t key_offset;
        format data_format;
        std::uint32_t data_length;
        std::uint32_t max_length;
        std::uint32_t data_offset;
    };

    struct header
    {
        std::uint32_t magic;
        std::uint32_t version;
        std::uint32_t key_offset;
        std::uint32_t data_offset;
        std::uint32_t total_entries;
    };

    std::optional<std::map<std::string, value>> load(std::istream& stream)
    {
        const auto head = svl::streams::read<header>(stream);

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

        map<string, value> val;

        for(int i = 0; i < head.total_entries; i++)
        {
            const auto redirect = svl::streams::read<index_table_entry>(stream);
            const auto dist = stream.tellg();

            stream.seekg(head.key_offset + redirect.key_offset);

            string key;

            while(auto c = stream.get())
                key += c;

            stream.seekg(head.data_offset + redirect.data_offset);

            const auto data = svl::streams::read_n(stream, redirect.max_length);

            value v = { redirect.data_format, std::move(data) };

            val.insert({ key, v });

            stream.seekg(dist);
        }

        return val;
    }
}