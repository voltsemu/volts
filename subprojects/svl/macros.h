#pragma once

#include "platform.h"

#if CL_MSVC
#   define VPACKED(name, ...) __pragma(pack(push)); __pragma(pack(1)); struct name __VA_ARGS__; __pragma(pack(pop));
#else
#   define VPACKED(name, ...) struct name __VA_ARGS__; 
#endif