#include "Render.h"
#include "Backends.h"
#include <Core/Collections/Array.h>
#include <Core/Collections/CthulhuString.h>

namespace Volts::PS3::RSX
{
    Cthulhu::Array<Render*>* Backends = nullptr;
    void Render::Register(Render* Backend)
    {
        // we create the array here to avoid the strange inner workings
        // of static initialization
        if(!Backends)
            Backends = new Cthulhu::Array<Render*>();

        // then we actually add the backend
        Backends->Append(Backend);
    }

    Cthulhu::Array<Render*> GetBackends()
    {
        return *Backends;
    }

    bool RenderDevice::operator==(const RenderDevice& Other) const
    {
        // comparing names should usually work
        // this can always be overloaded so this is just a fallback
        return Name() == Other.Name();
    }
}