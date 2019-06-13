#include "Render.h"
#include "Backends.h"
#include <Core/Collections/Array.h>
#include <Core/Collections/CthulhuString.h>

namespace Volts::PS3::RSX
{
    void Render::Register(Render* Backend)
    {
        MessageBox(nullptr, Backend->Name(), "Volts", 0);
    }

    Cthulhu::Array<Render*> GetBackends()
    {
        return {};
    }

    bool RenderDevice::operator==(const RenderDevice& Other) const
    {
        return Cthulhu::CString::Compare(Name(), Other.Name()) == 0;
    }
}