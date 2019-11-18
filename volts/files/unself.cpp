#include "unself.h"

#include "svl/types.h"
#include "svl/endian.h"

#include <sstream>

using svl::endian::big;

using namespace svl;

namespace volts::files::unself
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

        };

        //static_assert(sizeof(header) == 80);

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

                };
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

        }

        bool load_metadata()
        {

        }

        void decrypt()
        {

        }

        std::stringstream elf()
        {
            std::stringstream out;
        }

    private:
        std::istream& stream;
    };

    std::ostream load(std::istream& file, std::vector<byte> key)
    {

    }
}