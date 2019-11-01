#pragma once

#include <FileSystem/BufferedFile.h>

#include "Core/Endian.h"

#include "Core/Emulator.h"

namespace Volts::Utils
{
    namespace FS = Cthulhu::FileSystem;

    using namespace Cthulhu;

    namespace ELF
    {
        enum class Type : U16
        {
            None = 0,
            Rel = 1,
            Exec = 2,
            Dyn = 3,
            Core = 4,

            Prx = 65444,

            PSV1 = 65024,
            PSV2 = 65028,
        };

        enum class Machine : U16
        {
            PPC64 = 21,
            SPU = 23,
        };

        template<typename T>
        struct Header
        {
            U32 Magic;

            U8 Class;
            U8 Endian;
            U8 SVersion;
            U8 ABI;

            U8 ABIVersion;

            Byte Ident[7];

            Big<Type> Type;
            Big<Machine> Machine;
            Big<U32> Version;

            Big<T> Entry;
            Big<T> PHOffset;
            Big<T> SHOffset;

            Big<U32> Flags;
            Big<U16> HeaderSize;
            
            Big<U16> PHEntrySize;
            Big<U16> PHCount;

            Big<U16> SHEntrySize;
            Big<U16> SHCount;

            Big<U16> StringIndex;
        };

        static_assert(sizeof(Header<U32>) == 52);
        static_assert(sizeof(Header<U64>) == 64);

        template<typename T>
        struct ProgramHeader {};

        template<>
        struct ProgramHeader<U32>
        {
            Big<U32> Type;
            Big<U32> Offset;
            
            Big<U32> VirtualAddress;
            Big<U32> PhysicalAddress;
            
            Big<U32> FileSize;
            Big<U32> Align;
        };

        template<>
        struct ProgramHeader<U64>
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

        template<typename T>
        struct SectionHeader
        {
            Big<U32> Name;
            Big<U32> Type;
            
            Big<T> Flags;
            Big<T> Address;
            Big<T> Offset;
            Big<T> Size;

            Big<U32> Link;
            Big<U32> Info;

            Big<T> Align;
            Big<T> EntrySize;
        };

        template<typename T>
        struct Object
        {
            Header<T> Head;

            std::vector<ProgramHeader<T>> Progs;
            std::vector<SectionHeader<T>> Sections;
        };
    }

    template<typename T>
    ELF::Object<T>* LoadELF(FS::BufferedFile& File)
    {
        if(!File.Valid())
        {
            VERROR("ELF file was invalid");
            return nullptr;
        }

        auto* Out = new ELF::Object<T>();

        Out->Head = File.Read<ELF::Head<T>>();

        if(Out->Head.Magic != "\177ELF"_U32)
        {
            VERROR("Invalid ELF magic");
            return nullptr;
        }

        File.Seek(Out->Head.Offset)
        for(U32 I = 0; I < Out->Head.SHCount; I++)
        {
            Progs.push_back(File.Read<ELF::SectionHeader<T>>());
        }

        for(U32 I = 0; I < Out->Head.PHCount; I++)
        {
            Progs.push_back(File.Read<ELF::ProgramHeader<T>>());
        }

        return Out;
    }
}