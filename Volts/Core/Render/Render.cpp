#include "Render.h"

namespace Volts::RSX
{
    Empty AddBackend(Backend B)
    {
        Backends.Append(B);
        return {};
    }

    Array<Backend> Backends = Array<Backend>();
}