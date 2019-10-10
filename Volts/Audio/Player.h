#pragma once

#include <Core/Collections/Array.h>

namespace Volts::Audio
{
    using namespace Cthulhu;

    struct Player
    {
        virtual ~Player() {}
        virtual const char* Name() const = 0;
    };
}