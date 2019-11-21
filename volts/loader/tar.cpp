#include "tar.h"

#include <cstdlib>
#include <cmath>

#include "svl/stream.h"

using namespace svl;

namespace volts::loader::tar
{
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
        svl::pad padding[12];
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

    // ustar\20
    constexpr svl::byte ustar_magic[] = { 0x75, 0x73, 0x74, 0x61, 0x72, 0x20 };

    object load(std::shared_ptr<svl::iostream> stream)
    {
        object ret = stream;

        for(;;)
        {
            auto head = svl::read<header>(*stream);

            if(memcmp(head.magic, ustar_magic, sizeof(ustar_magic)))
                return ret;

            int size = octal_to_decimal(atoi(head.size));

            auto aligned = (size + stream->tell() + 512 - 1) & ~(512 - 1);

            ret.offsets[head.name] = stream->tell();

            stream->seek(aligned);
        }

        return ret;
    }
}