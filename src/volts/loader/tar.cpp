#include "tar.h"

#include <svl/types.h>

namespace volts::tar
{
    using namespace svl;

    struct Header
    {
        char name[100];
        char mode[8];
        char uid[8];
        char gid[8];
        char size[12];
        byte mtime[12];
        byte checksum[8];
        byte type;
        char link[100];

        byte magic[6];
        u8 version[2];
        char uname[32];
        char gname[32];
        char dev_major[8];
        char dev_minor[8];
        char prefix[155];
        pad<12> padding;
    };

    static_assert(sizeof(Header) == 512);

    int octal_to_decimal(const char* str)
    {
        int num = atoi(str);
        int ret = 0, i = 0, rem;

        while(num)
        {
            rem = num % 10;
            num /= 10;
            ret += rem * (int)pow(8, i++);
        }

        return ret;
    }

    svl::Option<svl::File> Object::get(const std::string& str)
    {
        if (offsets.find(str) == offsets.end())
        {
            return svl::none();
        }

        auto offset = offsets[str];

        source.seek(offset - sizeof(Header));

        const auto header = source.read<Header>();
        if (header.name != str)
        {
            return svl::none();
        }

        return svl::stream(source.read(octal_to_decimal(header.size)));
    }

    void Object::extract(const fs::path& path)
    {
        for (const auto& [name, offset] : offsets)
        {
            source.seek(offset - sizeof(Header));
            const auto header = source.read<Header>();

            if (header.type == '0')
            {
                svl::stream(source.read(octal_to_decimal(header.size)))
                    .save(path/name);
            }
            else if (header.type == '5')
            {
                fs::create_directories(path/name);
            }
            else
            {
                svl::log::error("[E0008] Invalid tar header");
            }
        }
    }

    constexpr svl::byte magic[] = { 0x75, 0x73, 0x74, 0x61, 0x72, 0x20 };

    Object load(svl::File&& source)
    {
        OffsetMap ret;

        while (true)
        {
            const auto header = source.read<Header>();

            if (memcmp(header.magic, magic, sizeof(magic)))
                break;

            ret[header.name] = source.tell();

            source.seek(ROUND2(source.tell() + octal_to_decimal(header.size), 512));
        }

        return { ret, std::move(source) };
    }
}