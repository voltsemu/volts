#pragma once

#include <filesystem>

namespace volts::vfs
{
    void set_root(const std::filesystem::path& new_root);
    std::filesystem::path get_root();
    std::filesystem::path get_path(const std::filesystem::path& path)
}