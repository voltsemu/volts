#pragma once

#include <external/wrapfs.h>

namespace volts::vfs
{
    // set the root of the virtual file system
    void set_root(const fs::path& new_root);

    // get the root path of the virtual file system
    fs::path get_root();

    // get a path inside the filesystem that is relative to root
    fs::path get_path(const fs::path& path);
}