#pragma once

#include <svl/platform.h>

#if SYS_WINDOWS
#   include "lmwwindows.h"
#elif SYS_OSX
#   include "lmwosx.h"
#else
#   include "lmwlinux.h"
#endif