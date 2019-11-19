#include "unself.h"

#include "svl/types.h"
#include "svl/endian.h"

#include <sstream>

using svl::endian::big;

using namespace svl;

namespace volts::loader::unself
{
    namespace sce
    {
        struct header
        {
            u32 magic;
            big<u32> version;
            big<u16> type;
            big<u16> category;
            big<u32> metadata_start;
            big<u64> metadata_end;
            big<u64> size;
        };

        static_assert(sizeof(header) == 32);
    }

    namespace self
    {
        struct header
        {
            big<u64> type;
            big<u64> app_info_offset;
            big<u64> elf_offset;
            big<u64> prog_header_offset;
            big<u64> section_header_offset;
            big<u64> section_info_offset;
            big<u64> version_offset;
            big<u64> control_offset;
            big<u64> control_length;
            pad padding[8];  
        };

        static_assert(sizeof(header) == 80);

        struct control_info
        {
            u32 type;
            u64 size;
            u64 next;

            union
            {
                struct 
                {
                    big<u32> flags;
                    pad padding[28];
                } control_flags;

                static_assert(sizeof(control_flags) == 32);

                struct 
                {
                    u8 constant[20];
                    u8 elf_digest[20];
                    big<u64> required_version;
                } elf_digest_40;

                static_assert(sizeof(elf_digest_40) == 48);

                struct
                {
                    u8 const_or_digest[20];
                    pad padding[12];
                } elf_digest_30;

                static_assert(sizeof(elf_digest_30) == 32);

                struct 
                {
                    u32 magic;
                    big<u32> version;
                    big<u32> drm_type;
                    big<u32> app_type;
                    byte content_id[48];
                    byte digest[16];
                    byte inv_digest[16];
                    byte xor_digest[16];
                    pad padding[16];
                } npdrm_info;

                static_assert(sizeof(npdrm_info) == 128);
            };
        };
    }

    struct self_decryptor
    {
        self_decryptor(std::istream& file)
            : stream(file)
        {}

        bool load_headers()
        {
            return true;
        }

        bool load_metadata(const std::vector<byte>& key)
        {
            return true;
        }

        void decrypt()
        {

        }

        std::vector<u8> elf()
        {
            //std::stringstream out;

            return {}; //out.rdbuf();
        }

    private:
        std::istream& stream;
    };

    std::optional<std::ostream> load(std::istream& file, std::vector<byte> key)
    {
        self_decryptor dec(file);

        if(!dec.load_headers())
        {
            return std::nullopt;
        }

        if(!dec.load_metadata(key))
        {
            return std::nullopt;
        }

        dec.decrypt();

        // todo: alot

        return std::nullopt;

        //return dec.elf();
    }
}