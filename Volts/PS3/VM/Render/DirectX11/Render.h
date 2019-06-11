#pragma once

#include "PS3/VM/Render/Render.h"

namespace Volts::PS3::RSX
{
    struct DirectX11 : Render
    {
        virtual ~DirectX11() override {}
    };
}
