#include "tar.h"

#include <string.h>
#include <math.h>

namespace vt::tar {
    struct header {
        char name[100];
        char mode[8];
        char uid[8];
        char gid[8];
        char size[12];
        u8 mtime[12];
        u8 checksum[8];
        char link[100];

        u8 magic[6];
        u8 version[2];
        char uname[32];
        char gname[32];
        char dev_major[8];
        char dev_minor[8];
        char prefix[155];
        char pad[12];
    };

    int oct2dec(int n) {
        int ret = 0;
        int i = 0;

        while (n) {
            auto rem = n % 10;
            n /= 10;
            ret += rem * (int)pow(8, i++);
        }

        return ret;
    }

    svl::file object::get(const std::string& path) {
        auto e = offsets[path];

        if (e.offset == 0) {
            return svl::buffer();
        }

        source.seek(e.offset);
        return svl::from(source.read<u8>(e.size));
    }

    void object::extract(const fs::path& dir) {
        fs::create_directories(dir);
    }

    u8 magic[] = { 0x75, 0x73, 0x74, 0x61, 0x72, 0x20 };

    object load(svl::file&& source) {
        std::map<std::string, object::entry> offsets;

        auto head = source.read<header>();

        while (!memcmp(head.magic, magic, sizeof(magic))) {

            auto size = oct2dec(atoi(head.size));
            auto here = source.tell();

            offsets[head.name] = { here, (u64)size };

            auto aligned = (size + here + 512 - 1) & ~(512 - 1);

            source.seek(aligned);

            head = source.read<header>();
        }

        return { offsets, std::move(source) };
    }
}
