#pragma once

#include <vector>
#include <optional>
#include <memory>

#include "svl/endian.h"
#include "svl/types.h"
#include "svl/file.h"

namespace volts::loader::pup
{
    /**
     * @brief a PUP object data table entry
     * always 32 bytes wide
     */
    struct entry
    {
        /// the id of the entry
        svl::endian::big<svl::u64> id; 

        /// the absolute offset of the data in the file
        svl::endian::big<svl::u64> offset; 

        /// the length of the data in bytes
        svl::endian::big<svl::u64> length; 

        /// structure padding
        svl::pad padding[8]; 
    };

    static_assert(sizeof(entry) == 32);

    /**
     * @brief a PUP object hash table entry
     * always 32 bytes wide
     */
    struct hash
    {
        /// the id of the assocciated entry
        svl::endian::big<svl::u64> id; 

        /// the hash of the entry
        svl::byte hash[20]; 

        /// padding
        svl::pad padding[4]; 
    };

    static_assert(sizeof(hash) == 32);

    /**
     * @brief a PUP (Playstation Update Package) object representation
     */
    struct object
    {
        /// all the files in the object
        std::vector<entry> files; 

        /// the hashes of the file entries
        std::vector<hash> hashes; 

        /**
         * @brief get a file from the update object
         * @param id the id of the entry to retrive
         * @return the extracted file
         */
        svl::file get_file(svl::u64 id);

        friend std::optional<object> load(svl::file);

    private:
        /**
         * @brief parse a PUP file
         * @param s the file to parse
         */
        object(svl::file s)
            : file(s)
        {}

        svl::file file; /// the source file of this PUP
    };

    /**
     * @brief parse a PUP file and return if the file was valid
     * @param stream the file to parse
     * @return the parsed object or an empty option
     */
    std::optional<object> load(svl::file stream);
}