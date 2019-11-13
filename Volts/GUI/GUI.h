#pragma once

#include "PCH.h"

#if OS_WINDOWS
#   include "Windows/GUI.h"
#elif OS_APPLE
#   include "Darwin/GUI.h"
#elif OS_LINUX
#   include "Linux/GUI.h"
#endif