#pragma once

#include <Cthulhu/Core/Collections/CthulhuString.h>

namespace Volts
{
    using namespace Cthulhu;

    struct Game
    {
        String Name;
        String ImagePath;
        String BootPath;
        String SavePath;
        String SFOPath;
    };
}