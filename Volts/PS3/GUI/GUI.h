#pragma once

#include <Meta/Macros.h>

#if OS_WINDOWS
#   include "Windows/GUI.h"
#elif OS_APPLE
#   include "Darwin/GUI.h"
#elif OS_LINUX
#   include "Linux/GUI.h"
#endif