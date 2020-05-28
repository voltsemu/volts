#pragma once

#include "tar.h"

#include <svl/types.h>

namespace volts::loader::tar
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

    Object load(svl::File&& source)
    {

    }
}