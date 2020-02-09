#pragma once

#include <vector>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <tchar.h>

#include <wrl.h>
using namespace Microsoft::WRL;

#include <external/alert/alert.h>

#define ENSURE(msg, ...)  { if((__VA_ARGS__) != S_OK) { alert::alert(L"d3d12 error", msg); return; } }

namespace volts::rsx::dxutil
{
    std::vector<ComPtr<IDXGIAdapter1>> list_adapters(ComPtr<IDXGIFactory> factory)
    {

    }
}