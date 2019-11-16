#include "PUP.h"

#include "Core/Convert.h"
#include "Core/Emulator.h"

namespace Volts::Utils
{
    namespace PUP
    {
        // the first 48 bytes of a PUp file is the header
        struct Header
        {
            // magic is always 302078313299 or "SCEUF\0\0\0"
            Little<U64> Magic;
            // ususally 1
            Big<U64> PackageVersion;
            // usually 66412
            Big<U64> ImageVersion;
            // the amount of files in the PUP object
            Big<U64> FileCount;
            // the size of the header + file table + hash table in bytes
            Big<U64> HeaderLength;
            // the length of the rest of the data
            Big<U64> DataLength;
        };

        static_assert(sizeof(Header) == 48);
    }

    namespace PUP
    {
        Memory::Binary Object::GetFile(U64 ID)
        {
            for(auto& Entry : Files)
            {
                if(Entry.ID == ID)
                {
                    File.Seek(Entry.Offset);
                    Memory::Binary Ret;
                    Types::Byte* Data = (Types::Byte*)alloca(sizeof(Types::Byte) * Entry.Length);
                    File.ReadN(Data, Entry.Length);
                    Ret.WriteN(Data, Entry.Length);
                    // dumb hack to make everything the right length internally
                    Ret.Seek(0);

                    return std::move(Ret);
                }
            }

            return {};
        }
    }
}