#include "tar.h"

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

    svl::file object::get(const std::string_view& path) {
        for (auto& [name, offset] : offsets) {
            if (name == path) {
                source.seek(offset);
                auto head = file.read<header>();
            }
        }
    }

    void object::extract(const fs::path& dir) {
        fs::create_directories(dir);
    }
}
