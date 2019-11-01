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
        V128 VR[32] = {};

        // condition registers
        bool CR[32] = {};

        template<typename T>
        void WriteCR(U32 Field, T L, T R)
        {
            const auto I = Field * 4;
            CR[I] = L < R;
            CR[I + 1] = L > R;
            CR[I + 2] = L == R;
            CR[I + 3] = XER.SO;
        }

        U64 LR = 0; // link register
        U64 CTR = 0; // count register
        U64 VRSave = 0; // vrsave
        U32 Cursor = 0; // current instruction address

        struct
        {
            bool SO;
            bool OV;
            bool CA;
            U8 Count;
        } XER = {};

        U64 FPSCR = 0;

        U32 StackSize = 0;
        U32 StackAddress = 0;
    };

    union PPUInstruction
    {
        U32 Raw;

        BitField<U32, 6, 24> LI;
        BitField<U8, 6, 5> TO;
        BitField<U32, 6, 5> RD;
        BitField<U32, 6, 5> BO;
        BitField<U32, 6, 3> BF;
        BitField<U32, 11, 5> BI;
        BitField<U32, 16, 14> BD;
        BitField<U32, 20, 7> LEV;
        BitField<U32, 6, 5> RT;
        BitField<U32, 11, 5> TA;
        BitField<U32, 16, 15> D;
        BitField<U32, 16, 15> SI;
        BitField<U32, 16, 15> UI;
        BitField<U32, 16, 14> DS;
        BitField<U32, 30, 1> XO;
        BitField<U32, 9, 1> L;
        BitField<U32, 6, 5> RS;

        BitField<U32, 30, 1> AA;
        BitField<U32, 31, 1> LK;

        BitField<U32, 16, 14> BT14;
        BitField<U32, 6, 24> BT24;
    };

    static_assert(sizeof(PPUInstruction) == sizeof(U32));
}