#pragma once

#include <external/wrapfs.h>

namespace volts::vfs
{
    /**
     * @brief set the current root of the virtual filesystem
     * 
     * @param new_root the new root
     */
    void root(const fs::path& new_root);

    /**
     * @brief get the current root of the vfs
     * 
     * @return fs::path the current vfs root
     */
    fs::path root();

    /**
     * @brief get a path relative to the vfs root
     * 
     * @param path the path to get
     * @return fs::path the corrected path
     */
    fs::path get(const fs::path& path);
}