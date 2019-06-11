#include "Render.h"
#include "Backends.h"
#include <Core/Collections/Array.h>

namespace Volts::PS3::RSX
{
    Cthulhu::Array<Render*> Backends = {};
    void Render::Register(Render* Backend)
    {
        Backends.Append(Backend);
    }

    Cthulhu::Array<Render*>& GetBackends()
    {
        return Backends;
    }
}