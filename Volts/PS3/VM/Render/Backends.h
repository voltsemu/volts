#pragma once

#include "Render.h"
#include <Core/Collections/Array.h>

namespace Volts::PS3::RSX
{
    // get all the possible RSX backends for this platform
    // could be something like OpenGL or DX12
    Cthulhu::Array<Render*> GetBackends();
}