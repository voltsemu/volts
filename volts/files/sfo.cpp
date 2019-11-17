#include "sfo.h"

#include "svl/stream.h"
#include "svl/convert.h"

namespace volts::files::sfo
{
    using namespace std;

    struct index_table_entry
    {
        const std::uint16_t key_offset;
        const format data_format;
        const std::uint32_t data_length;
        const std::uint32_t max_length;
        const std::uint32_t data_offset;
    };

    struct header
    {
        const std::uint32_t magic;
        const std::uint32_t version;
        const std::uint32_t key_offset;
        const std::uint32_t data_offset;
        const std::uint32_t total_entries;
    };

    std::optional<std::map<std::string, value>> load(std::fstream& stream)
    {
        // todo: logging
        const auto head = svl::streams::read<header>(stream);

        if(head.magic != svl::convert::to_u32("\0PSF"))
            return std::nullopt;

        if(head.version != 0x101)
            return std::nullopt;

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