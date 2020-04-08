#include "simple.h"

#include "horror.h"

namespace gac
{
    textblock text(const char* str)
    {
        textblock txt;
        txt.Text(winrt::to_hstring(str));
        return txt;
    }
}