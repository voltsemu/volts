#include "alert.h"

#ifdef _WIN32
#   include <windows.h>
#else
#   include <Xm/Xm.h>
#   include <Xm/PushB.h>
#endif

namespace alert
{
    void alert(const char* title, const char* message)
    {
#ifdef _WIN32
        MessageBoxA(nullptr, message, title, 0);
#else
        // TODO
#endif
    }
}