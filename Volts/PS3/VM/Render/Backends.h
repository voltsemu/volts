#pragma once

#include "Render.h"
#include <Core/Collections/Array.h>

namespace Volts::PS3::RSX
{
    Cthulhu::Array<Render*>& GetBackends();
}