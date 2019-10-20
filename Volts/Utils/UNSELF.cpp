#include "UNSELF.h"

#include "Core/Endian.h"

#include <vector>

namespace Volts::Utils
{
    namespace SCE
    {
        struct Header
        {
            U32 Magic;
            Big<U32> Version;
            Big<U16> Type;
            Big<U16> Category;
            Big<U32> MetadataStart;
            Big<U64> MetadataEnd;
            Big<U64> Size;
        };
    }

    static_assert(sizeof(SCE::Header) == 32);

    namespace ELF
    {
        struct SectionHeader
        {
            Big<U32> StringOffset;
            Big<U32> Type;

            Big<U64> Flags;
            Big<U64> VirtualAddress;
            Big<U64> Offset;
            Big<U64> Size;

            Big<U32> Link;
            Big<U32> Info;

            Big<U64> Align;
            Big<U64> EntrySize;
        };

        static_assert(sizeof(SectionHeader) == 64);

        struct ProgramHeader
        {
            Big<U32> Type;

            Big<U32> Flags;
            Big<U64> Offset;
            Big<U64> VirtualAddress;
            Big<U64> PhysicalAddress;
            Big<U64> FileSize;
            Big<U64> MemorySize;

            Big<U64> Align;
        };

        static_assert(sizeof(ProgramHeader) == 56);
    };

    struct Decryptor
    {
        FS::BufferedFile* Stream;

        Decryptor(FS::BufferedFile* File)
            : Stream(File)
        {}

    private:
        std::vector<ELF::SectionHeader> SHeaders;
        std::vector<ELF::ProgramHeader> PHeaders;
        
    public:

        bool ReadHeaders()
        {
            return false;
        }

        bool ReadMetadata()
        {
            return false;
        }

        void Decrypt()
        {
            
        }

        Cthulhu::Binary ToELF()
        {
            return {};
        }
    };

    Cthulhu::Binary LoadSELF(FS::BufferedFile&& File)
    {
        Decryptor Dec = &File;

        return Dec.ToELF();
    }   
}