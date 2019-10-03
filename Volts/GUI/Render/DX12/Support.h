#pragma once

#include "Volts/Core/Logger/Logger.h"
#include "Config.h"
#include "Render/Render.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <wrl.h>
#include <shellapi.h>
#include <comdef.h>

#define DX_DEBUG(Queue, ...) Queue->AddApplicationMessage(D3D12_MESSAGE_SEVERITY_MESSAGE, fmt::format(__VA_ARGS__).c_str())
#define VALIDATE(...) if(FAILED(__VA_ARGS__)) { DX_DEBUG(DebugQueue, "DX12 Error {}", GetLastError()); VFATAL("[%s]:%s DX12 Call failed", __FILE__, __LINE__); exit(1); }


namespace Volts::DX12Support
{
    template<typename T>
    using Ptr = Microsoft::WRL::ComPtr<T>;

    struct DX12Device : RSX::Device
    {
        DX12Device() {}
        DX12Device(IDXGIAdapter1* Dev)
            : Handle(Dev)
        {
            Dev->GetDesc(&Desc);
            wcstombs(DeviceName, Desc.Description, 128);
        }

        virtual const char* Name() const override
        {
            return "aaa";
        }
    private:
        IDXGIAdapter1* Handle;
        DXGI_ADAPTER_DESC Desc;
        char DeviceName[128];
    };

    bool CanTear();
}