#pragma once

#include "PS3/VM/Render/Render.h"

namespace Volts::PS3::RSX
{
    struct DirectX12 : Render
    {
        virtual ~DirectX12() override {}
    };
}
