#pragma once

#include "Audio/Player.h"

namespace Volts::Audio
{
    struct OpenAL : Player
    {
        virtual ~OpenAL() override {}
        virtual const char* Name() const override { return "OpenAL"; }
    };
}