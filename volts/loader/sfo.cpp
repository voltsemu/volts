#include "sfo.h"

#include <svl/cvt.h>
#include <svl/file.h>

namespace vt::sfo {
    struct header {
        u32 magic;
        little<u32> version;
        little<u32> keys;
        little<u32> data;
        little<u32> entries;
    };

    enum struct format : u16 {
        array = 4,
        string = 516,
        integer = 1028
    };

    struct table_entry {
        little<u16> key;
        little<format> fmt;
        little<u32> len;
        little<u32> max;
        little<u32> offset;
    };

    std::optional<data> load(svl::file&& file) {
        auto head = file.read<header>();

        if (head.magic != cvt::u32("\0PSF") || head.version != 0x101) {
            return std::nullopt;
        }

        data fields;

        for (auto redirect : file.read<table_entry>(head.entries)) {
            file.seek(redirect.key + head.keys);

            std::string key;
            while (auto c = file.read<char>())
                key += c;

            file.seek(redirect.offset + head.data);

            entry data;

            switch (redirect.fmt) {
            case format::array:
                data = file.read<u8>(redirect.max);
                break;
            case format::integer:
                data = file.read<u32>();
                break;
            case format::string: {
                auto vec = file.read<char>(redirect.max);
                data = std::string(vec.begin(), vec.end());
                break;
            }
            default:
                return std::nullopt;
            }

            fields[key] = data;
        }

        return fields;
    }

    u32 data_size(const entry& item) {
        return std::visit(visitor {
            [](u32) { return 4u; },
            [](const std::vector<u8>& vec) { return (u32)vec.size(); },
            [](const std::string& str) { return (u32)str.size() + 1; }
        }, item);
    }

    format data_type(const entry& item) {
        return std::visit(visitor {
            [](u32) { return format::integer; },
            [](const std::vector<u8>&) { return format::array; },
            [](const std::string&) { return format::string; }
        }, item);
    }

    svl::file save(const data& items) {
        std::vector<table_entry> offsets;

        auto keys = svl::buffer();
        auto data = svl::buffer();

        for (auto& [key, it] : items) {
            auto size = data_size(it);

            offsets.push_back({
                (u16)keys.tell(), data_type(it),
                size, size,
                (u32)data.tell()
            });

            keys.write(key.data(), key.size());
            keys.write('\0');

            std::visit(visitor {
                [&](u32 num) { data.write(num); },
                [&](const std::vector<u8>& vec) { data.write(vec); },
                [&](const std::string& str) {
                    data.write(str.data(), str.size());
                    data.write('\0');
                }
            }, it);
        }

        auto size = items.size();

        header head = {
            cvt::u32("\0PSF"),
            0x101,
            u32(size * sizeof(table_entry) + sizeof(header)),
            u32(keys.size() + size * sizeof(table_entry) + sizeof(header)),
            u32(size)
        };

        auto out = svl::buffer();

        out.write(head);
        out.write(offsets);
        out.insert(std::move(keys));
        out.insert(std::move(data));

        return out;
    }
}