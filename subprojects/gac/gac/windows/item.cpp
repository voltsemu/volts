#pragma once

#include "item.h"

namespace gac
{
    navitem item(const char* label)
    {
        navitem i;
        i.Content(winrt::box_value(winrt::to_hstring(label)));
        return i;
    }
}