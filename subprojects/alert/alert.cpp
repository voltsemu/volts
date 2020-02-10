#include "alert.h"

#ifdef _WIN32
#   include <windows.h>
#else
#   include <Xm/Xm.h>
#   include <Xm/PushB.h>
#endif

namespace alert
{
    void alert(const std::wstring& title, const std::wstring& message)
    {
#ifdef _WIN32
        MessageBoxW(nullptr, message.c_str(), title.c_str(), 0);
#else
        // TODO
#endif
    }
}