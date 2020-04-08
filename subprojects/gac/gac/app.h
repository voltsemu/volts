#pragma once

#include <platform.h>

#if SYS_WINDOWS
#   include "windows/app.h"
#elif SYS_OSX
#   include "darwin/app.h"
#elif SYS_UNIX
#   include "linux/app.h"
#endif