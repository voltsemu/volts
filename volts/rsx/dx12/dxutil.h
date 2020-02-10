#pragma once

#include <vector>

#include <d3d12.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <tchar.h>

#include <wrl.h>
using namespace Microsoft::WRL;

#include <external/alert/alert.h>

#define VALIDATE()

#define ENSURE(msg, ...)  { if((__VA_ARGS__) != S_OK) { alert::alert(L"d3d12 error", msg); return; } }

namespace volts::rsx::dxutil
{
    void get_adapter(IDXGIFactory2* factory, IDXGIAdapter1** adapt)
    {
        ComPtr<IDXGIAdapter1> adapter;
        *adapt = nullptr;

        for(UINT idx = 0; factory->EnumAdapters1(idx, &adapter) != DXGI_ERROR_NOT_FOUND; idx++)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                continue;

            if(SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }

        *adapt = adapter.Detach();
    }
}