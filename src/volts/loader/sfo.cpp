#include "sfo.h"

#include <svl/traits.h>
#include <svl/convert.h>
#include <svl/panic.h>

namespace volts::sfo
{
    namespace cvt = svl::convert;
    using namespace svl;

    enum class Format : u16
    {
        string = 516,
        array = 4,
        integer = 1028
    };

    struct IndexTableEntry
    {
        u16 key_offset;
        Format data_format;
        u32 data_length;
        u32 max_length;
        u32 data_offset;
    };

    struct Header
    {
        u32 magic;
        u32 version;
        u32 key_offset;
        u32 data_offset;
        u32 entries;
    };

    Value make_value(std::vector<byte>&& data, Format format)
    {
        switch (format)
        {
        case Format::array: return std::move(data);
        case Format::string: return std::string(data.begin(), data.end());
        case Format::integer: return *(i32*)data.data();
        default: svl::panic("[E0001]: Invalid SFO data format {}", format);
        }
    }

    Format get_format(const Value& val)
    {
        return std::visit(svl::Visitor {
            [](const std::vector<byte>&) { return Format::array; },
            [](i32) { return Format::integer; },
            [](const std::string&) { return Format::string; },
            [](const auto&) { svl::panic("[E0002] Invalid SFO data type"); } // TODO: error out
        }, val);
    }

    u32 get_size(const Value& val)
    {
        return std::visit(svl::Visitor {
            [](const std::vector<byte>& arr) { return static_cast<u32>(arr.size()); },
            [](const i32&) { return (u32)sizeof(i32); },
            [](const std::string& str) { return static_cast<u32>(str.size() + 1); },
            [](const auto&) { svl::panic("[E0002] Invalid SFO data type"); } // TODO: error out
        }, val);
    }

    svl::Option<Object> load(svl::File&& source)
    {
        const auto header = source.read<Header>();

        Object out;

        const auto redirects = source.read<IndexTableEntry>(header.entries);

        for (const auto& redirect : redirects)
        {
            source.seek(header.key_offset + redirect.key_offset);

            std::string key;

            while (char c = source.read<char>())
                key += c;

            source.seek(header.data_offset + redirect.data_offset);

            auto data = source.read(redirect.max_length);

            out.insert({ key, make_value(std::move(data), redirect.data_format) });
        }

        return std::move(out);
    }

    struct Entry
    {
        u16 key;
        u32 data;
        u32 size;
        Format type;
    };

    svl::File unload(const Object& sfo)
    {
        std::vector<Entry> offsets = {};
        svl::File keys = svl::buffer();
        svl::File data = svl::buffer();

        auto out = svl::buffer();

        for (const auto& [key, val] : sfo)
        {
            offsets.push_back({
                static_cast<u16>(keys.tell()),
                static_cast<u16>(data.tell()),
                static_cast<u32>(get_size(val)),
                get_format(val)
            });

            keys.write(key);
            data.write(val);
        }

        const u32 entries = static_cast<u32>(offsets.size());

        out.write<Header>({
            cvt::to_u32("\0PSF"),
            0x101,
            static_cast<u32>(entries * sizeof(IndexTableEntry) + sizeof(Header)),
            static_cast<u32>(keys.size() + entries * sizeof(IndexTableEntry) + sizeof(Header)),
            entries
        });

        for (const auto& offset : offsets)
        {
            out.write<IndexTableEntry>({
                offset.key,
                offset.type,
                offset.size,
                offset.size,
                offset.data
            });
        }

        out.insert(std::move(keys));
        out.insert(std::move(data));

        return std::move(out);
    }
}