#pragma once

#include "Volts/Core/Logger/Logger.h"

#include "Config.h"

#define VALIDATE(...) if(FAILED(__VA_ARGS__)) { VFATAL("[%s]:%s DX12 Call failed", __FILE__, __LINE__); exit(1); }


namespace Volts::DX12Support
{
    bool CanTear();
}