#pragma once

#include "Input/Controller.h"

namespace Volts::Input
{
    struct MKB : Controller
    {
        virtual ~MKB() override {}
        virtual const char* Name() const override { return "Mouse+Keyboard"; }
    };
}