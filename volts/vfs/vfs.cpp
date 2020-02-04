#include "vfs.h"

namespace volts::vfs
{
    fs::path root = fs::current_path()/"vfs";

    void set_root(const fs::path& new_path)
    {
        root = new_path;
    }

    fs::path get_root()
    {
        return root;
    }

    fs::path get_path(const fs::path& path)
    {
        return root/path;
    }
}