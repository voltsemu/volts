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

    svl::file object::get_file(std::string name)
    {
        if(offsets.find(name) == offsets.end())
        {
            return svl::from({});
        }
        
        auto offset = offsets[name];
        file.seek(offset - sizeof(header));
        auto head = file.read<header>();
        if(strcmp(head.name, name.c_str()) == 0)
        {
            return svl::from(file.read<u8>(octal_to_decimal(atoi(head.size))));
        }

        return svl::from({});
    }

    void object::extract(const fs::path& to)
    {
        for(auto& [name, offset] : offsets)
        {
            file.seek(offset - sizeof(header));
            const auto head = file.read<header>();

            switch(head.file_type)
            {
                case '0':
                    open(to/name, svl::mode::write)
                        .write<u8>(file.read<u8>(octal_to_decimal(atoi(head.size))));
                    break;
                case '5':
                    fs::create_directories(to/name);
                    break;
                default:
                    spdlog::error("invalid tar section");
                    break;
            }
        }
    }

    // ustar\20
    constexpr svl::byte ustar_magic[] = { 0x75, 0x73, 0x74, 0x61, 0x72, 0x20 };

    object load(svl::file stream)
    {
        object ret = stream;

        for(;;)
        {
            auto head = stream.read<header>();

            if(memcmp(head.magic, ustar_magic, sizeof(ustar_magic)))
                return ret;

            int size = octal_to_decimal(atoi(head.size));

            auto aligned = (size + stream.tell() + 512 - 1) & ~(512 - 1);

            ret.offsets[head.name] = stream.tell();

            stream.seek(aligned);
        }

        return ret;
    }
}