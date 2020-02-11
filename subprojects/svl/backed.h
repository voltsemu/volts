#pragma once

#include "file.h"
#include "wrapfs.h"

#include <map>

namespace svl
{
    struct backed_value
    {
        std::string operator=(const std::string& val);

        operator std::string() const { return val; }

    private:
        friend struct backed;
        std::string val;
        std::string name;

        backed* parent;
    };

    enum class load_error 
    {
        // loaded properly
        ok,

        // file failed to open
        file,
    };

    /**
     * @brief a structure backed by an ini file
     * 
     * the saved file looks like an ini file but without sections
     * and every pair ends with `;`
     */
    struct backed
    {
        backed(const fs::path& path);
        ~backed();

        backed_value operator[](const std::string& val);

        /**
         * @brief load changes from disk
         * 
         */
        load_error load();

        /**
         * @brief save changes to disk
         * 
         */
        void save();

    private:
        std::map<std::string, std::string> data;
        fs::path path;
    };
}