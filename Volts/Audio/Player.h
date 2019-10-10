#pragma once

#include <Core/Collections/Array.h>

namespace Volts::Audio
{
    using namespace Cthulhu;

    struct Player
    {
        virtual ~Player() = 0;
        virtual const char* Name() const = 0;
    };
}