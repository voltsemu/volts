#include "vfs.h"

namespace volts::vfs
{
    std::filesystem::path root = std::filesystem::current_path()/"vfs";

    void set_root(const std::filesystem::path& new_path)
    {
        root = new_path;
    }

    std::filesystem::path get_root()
    {
        return root;
    }

    std::filesystem::path get_path(const std::filesystem::path& path)
    {
        return root/path;
    }
}