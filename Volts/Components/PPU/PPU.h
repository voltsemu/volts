#pragma once

#include "Core/Types.h"

namespace Volts::PPU
{
    using namespace Cthulhu;

    using namespace Types;

    struct PPU
    {
        virtual ~PPU() {}
        virtual const char* Name() const = 0;
    };

    template<typename T>
    struct AddResult
    {
        AddResult(T L, T R)
            : Result(L + R)
            , Carry(Result < L)
            , Zero(Result == 0)
            , Sign(Result >> (sizeof(T) * 8 - 1) != 0)
        {}

        AddResult(T L, T R, bool B)
            : AddResult(L, R)
        {
            AddResult Res(Result, B);
            Result = Res.Result;
            Carry |= Res.Carry;
            Zero = Res.Zero;
            Sign = Res.Sign;
        }

        static AddResult<T> Flags(T L, T R)
        {
            return {L, R};
        }

        static AddResult<T> Flags(T L, T R, bool B)
        {
            return {L, R, B};
        }

        T Result;

        bool Carry, Zero, Sign;
    };

    struct PPUThread
    {
        // general registers
        U64 GPR[32] = {};

        // floating point registers
        F64 FPR[32] = {};

        // vector registers
        // TODO: implement v128 type
        // 128 bit unsigned integer
        V128 VR[32] = {};

        // condition registers
        bool CR[32] = {};

        template<typename T>
        void WriteCR(U32 Field, T L, T R)
        {
            CR[Field * 4] = L < R;
            CR[Field * 4 + 1] = L > R;
            CR[Field * 4 + 2] = L == R;
            CR[Field * 4 + 3] = XER.SO;
        }

        U64 LR = 0; // link register
        U64 CTR = 0; // count register
        U64 VRSave = 0; // vrsave
        U32 Cursor = 0; // current instruction address

        struct
        {
            bool SO, OV, CA;
            U8 Count;
        } XER = {};

        U64 FPSCR = 0;

        U32 StackSize = 0;
        U32 StackAddress = 0;
    };

    struct PPUInstruction
    {
        U32 Raw;
    };

    static_assert(sizeof(PPUInstruction) == sizeof(U32));
}