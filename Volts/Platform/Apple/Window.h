#pragma once

#include "Cthulhu/Core/Collections/Array.h"
#include "Cthulhu/Core/Collections/CthulhuString.h"

namespace Volts
{
    using namespace Cthulhu;

    struct DropdownButton
    {

    };

    struct DropdownMenu
    {
        Array<DropdownButton> Options;
    };

    struct EmulatorWindow
    {
        using Self = EmulatorWindow;

        Self& AddMenu(const DropdownMenu Menu);
        Self& AddGame();
    private:
        void* Handle;
    };
}