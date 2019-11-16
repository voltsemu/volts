#pragma once

#include "PCH.h"

namespace Volts::Utils
{
    using namespace Cthulhu;

    Cthulhu::Memory::Binary LoadSELF(Interfaces::Stream&& File, Types::Byte* Key = nullptr);
}