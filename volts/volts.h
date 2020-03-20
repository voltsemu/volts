#pragma once

#define V_ASSERT(expr) { if(!(expr)) { spdlog::critical("assert failed {}", #expr); std::abort(); } }

#include <wrapfs.h>
#include <string>
#include <map>
