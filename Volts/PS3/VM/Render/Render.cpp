#include "Render.h"
#include "Backends.h"
#include <Core/Collections/Array.h>
#include <Core/Collections/CthulhuString.h>

namespace Volts::PS3::RSX
{
    Cthulhu::Array<Render*> Backends;
    Cthulhu::U32 Num = 0;
    void Render::Register(Render* Backend)
    {
        Num++;
        printf("Num: %u Len: %u\n", Num, Backends.Len());
        Backends.Append(Backend);

        for(auto& A : Backends)
        {
            printf("R: %s\n", A->Name());
        }
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