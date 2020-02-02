#pragma once

#include <map>
#include <string>

#include "svl/types.h"
#include "svl/file.h"

namespace volts::loader::tar
{
    /**
     * @brief tar file structure
     * 
     */
    struct object
    {
        /// map of file names to file offsets
        std::map<std::string, svl::u64> offsets;

        /**
         * @brief get a file out of the tar file
         * 
         * @param name the name of the file to find
         * @return svl::file the file if it was found, otherwise the stream is empty
         */
        svl::file get_file(std::string name);

        /**
         * @brief extract the tar file to a path on disk
         * 
         * @param to the location to extract to
         */
        void extract(const std::filesystem::path& to);

        friend object load(svl::file stream);

    private:
        object(svl::file s)
            : file(s)
        {}

        /// the backing file of this object
        svl::file file;
    };

    /**
     * @brief parse a tar object from a file
     * 
     * @param stream the file to read from
     * @return object the parsed object (any file is a valid tar file so it will always return data)
     */
    object load(svl::file stream);
}