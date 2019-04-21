#include "Render.h"

namespace RPCS3X::RSX
{
    Empty AddBackend(const RenderInfo Info)
    { 
        Backends.Append(Info); 
        return {}; 
    }
}