#include "Render.h"
#include "Backends.h"
#include <Core/Collections/Array.h>
#include <Core/Collections/CthulhuString.h>

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

    bool RenderDevice::operator==(const RenderDevice& Other) const
    {
        return Cthulhu::CString::Compare(Name(), Other.Name()) == 0;
    }
}