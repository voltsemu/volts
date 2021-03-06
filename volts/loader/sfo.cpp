#include "sfo.h"

#include <svl/convert.h>

#include <spdlog/spdlog.h>

namespace volts::loader::sfo
{
    using namespace std;
    using namespace svl;
    namespace cvt = convert;

    /**
     * @brief an entry in the sfo file
     * 
     */
    struct index_table_entry
    {
        /// the offset of the key relative to sfo::header::key_offset
        u16 key_offset;

        /// the format of the data for this key
        format data_format;

        /// the length of the data
        u32 data_length;

        /// the maximum amount of bytes this data can use
        u32 max_length;

        /// the offset of the data relative to sfo::header::data_offset
        u32 data_offset;
    };

    /**
     * @brief the sfo header
     * 
     */
    struct header
    {
        /// file magic, always "\0PSF"
        u32 magic;

        /// the version of the sfo file, we only support 0x101 for now
        u32 version;

        /// the offset of the key table
        u32 key_offset;

        /// the offset of the data entries
        u32 data_offset;

        /// the total number of entries in the file
        u32 total_entries;
    };

    svl::expected<object> load(svl::file stream)
    {
        // read in the header
        const auto head = stream.read<header>();

        // make sure this is actually an sfo file
        if(head.magic != cvt::to_u32("\0PSF"))
        {
            spdlog::error("invalid magic");
            return svl::none();
        }

        // make sure this is a supported version
        if(head.version != 0x101)
        {
            spdlog::error("unsupported version");
            return svl::none();
        }

        object val;

        // read in the redirectors
        const auto redirects = stream.read<index_table_entry>(head.total_entries);

        // for every entry
        for(const auto redirect : redirects)
        {
            // seek to the key in the key table
            stream.seek(head.key_offset + redirect.key_offset);

            string key;

            // read in the key, its always null terminated
            while(auto c = stream.read<char>())
                key += c;

            // seek to the data in the data table
            stream.seek(head.data_offset + redirect.data_offset);

            // read in the data
            const auto data = stream.read<svl::byte>(redirect.max_length);

            value v = { redirect.data_format, std::move(data) };

            // put the data into the object
            val.insert({ key, v });
        }

        return val;
    }

    void write(svl::file& stream, const object& obj)
    {        
        struct triple { u16 key; u32 data; u32 size; format fmt; };
        
        // offsets
        std::vector<triple> offsets = {};
        // data
        svl::file keys = svl::buffer();
        svl::file data = svl::buffer();

        // write data to buffers
        for(auto& [key, it] : obj)
        {
            offsets.push_back({
                static_cast<u16>(keys.tell()),
                static_cast<u32>(data.tell()),
                static_cast<u32>(it.data.size()),
                it.type
            });

            keys.write(key, true);
            data.write(it.data);
        }

        auto size = obj.size();

        // create header
        header head = {
            cvt::to_u32("\0PSF"), // magic
            0x101, // version
            static_cast<u32>(size * sizeof(index_table_entry) + sizeof(header)), // key table offset
            static_cast<u32>(keys.size() + size * sizeof(index_table_entry) + sizeof(header)), // data offset
            static_cast<u32>(size) // total entries
        };

        stream.seek(0);
        stream.write(head);
        for(auto idx : offsets)
        {
            index_table_entry entry = {
                idx.key,
                idx.fmt,
                idx.size,
                idx.size,
                idx.data
            };

            stream.write(entry);
        }

        stream.insert(keys);
        stream.insert(data);
    }
}