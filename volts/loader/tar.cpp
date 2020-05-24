#include "tar.h"

#include <spdlog/spdlog.h>
#include <algorithm>

#include <cstdlib>
#include <cmath>

namespace volts::loader::tar
{
    using namespace svl;

    struct header
    {
        char name[100];
        char mode[8];
        char uid[8];
        char gid[8];
        char size[12];
        svl::byte mtime[12];
        svl::byte checksum[8];
        svl::byte file_type;
        char link[100];

        svl::byte magic[6];
        svl::u8 version[2];
        char uname[32];
        char gname[32];
        char dev_major[8];
        char dev_minor[8];
        char prefix[155];
        svl::padding<12> padding;
    };

    static_assert(sizeof(header) == 512);

    int octal_to_decimal(int num)
    {
        int ret = 0, i = 0, rem;

        while(num)
        {
            rem = num % 10;
            num /= 10;
            ret += rem * (int)pow(8, i++);
        }

        return ret;
    }

    svl::file object::get_file(const char* name)
    {
        if(offsets.find(name) == offsets.end())
        {
            return svl::buffer();
        }

        auto offset = offsets[name];
        file.seek(offset - sizeof(header));
        auto head = file.read<header>();
        if(strcmp(head.name, name) == 0)
        {
            return svl::from(file.read<u8>(octal_to_decimal(atoi(head.size))));
        }

        return svl::buffer();
    }

    void object::extract(const fs::path& to)
    {
        for(auto& [name, offset] : offsets)
        {
            file.seek(offset - sizeof(header));
            const auto head = file.read<header>();

            if(head.file_type == '0')
            {
                auto data = file.read<u8>(octal_to_decimal(atoi(head.size)));
                auto file = open(to/name, svl::mode::write);
                file.write<u8>(data);
            }
            else if(head.file_type == '5')
            {
                fs::create_directories(to/name);
            }
            else
            {
                spdlog::error("invalid tar section");
            }
        }
    }

    // ustar\20
    constexpr svl::byte ustar_magic[] = { 0x75, 0x73, 0x74, 0x61, 0x72, 0x20 };

    object load(svl::file stream)
    {
        object ret = stream;

        stream.save("temp.tar");

        spdlog::info("size: {}", stream.size());

        for(;;)
        {
            auto head = stream.read<header>();

            if(memcmp(head.magic, ustar_magic, sizeof(ustar_magic)))
                break;

            int size = octal_to_decimal(atoi(head.size));

            auto aligned = (size + stream.tell() + 512 - 1) & ~(512 - 1);

            ret.offsets[head.name] = stream.tell();

            stream.seek(aligned);
            spdlog::info("aligned {}", aligned);
        }

        spdlog::info("end {}", stream.tell());

        for(auto& [key, val] : ret.offsets)
        {
            spdlog::info("{}: {}", key, val);
        }

        return ret;
    }
}