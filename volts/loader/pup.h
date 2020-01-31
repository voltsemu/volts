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
     * A PUP object data table entry
     * always 32 bytes wide
     */
    struct entry
    {
        svl::endian::big<svl::u64> id; /// the id of the entry
        svl::endian::big<svl::u64> offset; /// the absolute offset of the data in the file
        svl::endian::big<svl::u64> length; /// the length of the data in bytes
        svl::pad padding[8]; /// structure padding
    };

    static_assert(sizeof(entry) == 32);

    /**
     * A PUP object hash table entry
     * always 32 bytes wide
     */
    struct hash
    {
        svl::endian::big<svl::u64> id; /// the id of the assocciated entry
        svl::byte hash[20]; /// the hash of the entry
        svl::pad padding[4]; /// padding
    };

    static_assert(sizeof(hash) == 32);

    /**
     * A PUP (Playstation Update Package) object representation
     */
    struct object
    {
        std::vector<entry> files; /// all the files in the object
        std::vector<hash> hashes; /// the hashes of the file entries

        /**
         * Get a file from the update object
         * @param id the id of the entry to retrive
         * @return the extracted file
         */
        svl::file get_file(svl::u64 id);

        friend std::optional<object> load(svl::file stream);

    private:
        /**
         * Parse a PUP file
         * @param s the file to parse
         */
        object(svl::file s)
            : file(s)
        {}

        svl::file file; /// the source file of this PUP
    };

    /**
     * parse a PUP file and return if the file was valid
     * @param stream the file to parse
     * @return the parsed object or an empty option
     */
    std::optional<object> load(svl::file stream);
}