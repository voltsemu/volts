#pragma once

#include <map>
#include <string>

#include <svl/types.h>
#include <svl/file.h>
#include <svl/wrapfs.h>

namespace volts::loader::tar
{
    using offset_map = std::map<std::string, svl::u64>;

    /**
     * @brief tar file structure
     *
     */
    struct object
    {
        /// map of file names to file offsets
        offset_map offsets;

        /**
         * @brief get a file out of the tar file
         *
         * @param name the name of the file to find
         * @return svl::file the file if it was found, otherwise the stream is empty
         */
        svl::file get_file(const char* name);

        /**
         * @brief extract the tar file to a path on disk
         *
         * @param to the location to extract to
         */
        void extract(const fs::path& to);

        friend object load(svl::file stream);

    private:
        object(offset_map off, svl::file s)
            : offsets(off)
            , file(s)
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