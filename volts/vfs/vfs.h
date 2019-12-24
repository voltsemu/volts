#pragma once

#include <filesystem>

namespace volts::vfs
{
    // set the root of the virtual file system
    void set_root(const std::filesystem::path& new_root);

    // get the root path of the virtual file system
    std::filesystem::path get_root();

    // get a path inside the filesystem that is relative to root
    std::filesystem::path get_path(const std::filesystem::path& path);
}