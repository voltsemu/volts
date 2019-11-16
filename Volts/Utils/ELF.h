#pragma once

#include "PCH.h"

#include "Core/Endian.h"

#include "Core/Emulator.h"

namespace Volts::Utils
{
    using namespace Cthulhu;
    using namespace Types;

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

        enum class OS : U8
        {
            None = 0,

            LV2 = 102
        };

        template<typename T>
        struct Header
        {
            U32 Magic;

            U8 Class;
            U8 Endian;
            U8 SVersion;
            OS ABI;

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

        template<typename T, Type TType, OS TSystem, Machine TMachine, typename TStream>
        struct Object
        {
            Interfaces::Stream<TStream> Data;
            Header<T> Head;

            std::vector<ProgramHeader<T>> Progs;
            std::vector<SectionHeader<T>> Sections;
        };
    }

    using PPUExec = ELF::Object<U64, ELF::Type::Exec, ELF::OS::None, ELF::Machine::PPC64>;
    using PPCPRX = ELF::Object<U64, ELF::Type::Prx, ELF::OS::LV2, ELF::Machine::PPC64>;
    using SPUExec = ELF::Object<U32, ELF::Type::Exec, ELF::OS::None, ELF::Machine::SPU>;

    template<typename T, ELF::Type TType, ELF::OS TSystem, ELF::Machine TMachine, typename TStream>
    std::optional<ELF::Object<T, TType, TSystem, TMachine>> LoadELF(Interfaces::Stream<TStream>& File)
    {
        auto Out = ELF::Object<T, TType, TSystem, TMachine>();

        Out.Head = File.Read<ELF::Header<T>>();

        if(Out.Head.Magic != "\177ELF"_U32)
        {
            VERROR("Invalid ELF magic");
            return std::nullopt;
        }

        File.Seek(Out.Head.SHOffset);
        for(U32 I = 0; I < Out.Head.SHCount; I++)
            Out.Sections.push_back(File.Read<ELF::SectionHeader<T>>());

        File.Seek(Out.Head.PHOffset);
        for(U32 I = 0; I < Out.Head.PHCount; I++)
            Out.Progs.push_back(File.Read<ELF::ProgramHeader<T>>());

        File.Seek(0);
        auto* Data = alloca(File.Length());
        File.WriteN(Data, File.Length());
        Out.Data.ReadN(Data, File.Length());

        return std::move(Out);
    }
}