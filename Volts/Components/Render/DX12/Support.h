#pragma once

#include "Core/Emulator.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <wrl.h>
#include <shellapi.h>
#include <comdef.h>

namespace Volts::Render
{
    template<typename T>
    using Ptr = Microsoft::WRL::ComPtr<T>;

    struct DX12Device : Device
    {
        DX12Device() {}
        DX12Device(IDXGIAdapter1* Dev)
            : Handle(Dev)
        {
            Handle->GetDesc(&Desc);
            wcstombs(DeviceName, Desc.Description, 128);
        }

        virtual ~DX12Device() {}

        virtual const char* Name() const override
        {
            return DeviceName;
        }

        IDXGIAdapter1* Handle;
        DXGI_ADAPTER_DESC Desc;
        char DeviceName[128];
    };

    bool CanTear()
    {
        Ptr<IDXGIFactory4> Factory;
        auto Res = CreateDXGIFactory1(IID_PPV_ARGS(&Factory));
        bool Tearing = false;
        if(SUCCEEDED(Res))
        {
            Ptr<IDXGIFactory5> Factory5;
            Res = Factory.As(&Factory5);
            if(SUCCEEDED(Res))
            {
                Res = Factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &Tearing, sizeof(Tearing));
            }
        }

        return SUCCEEDED(Res) && Tearing;
    }
}