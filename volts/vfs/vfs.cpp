#include "vfs.h"

namespace volts::vfs
{
    fs::path vfs_root = fs::current_path()/"vfs";

    void root(const fs::path& new_root)
    {
        vfs_root = new_root;
    }

    fs::path root()
    {
        return vfs_root;
    }

    fs::path get(const fs::path& path)
    {
        return vfs_root/path;
    }
}