#include "Support.h"

#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d12.h>

namespace Volts::DX12Support
{
    using Microsoft::WRL::ComPtr;
    bool CanTear()
    {
        ComPtr<IDXGIFactory4> Factory;
        auto Res = CreateDXGIFactory1(IID_PPV_ARGS(&Factory));
        bool Tearing = false;
        if(SUCCEEDED(Res))
        {
            ComPtr<IDXGIFactory5> Factory5;
            Res = Factory.As(&Factory5);
            if(SUCCEEDED(Res))
            {
                Res = Factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &Tearing, sizeof(Tearing));
            }
        }

        return SUCCEEDED(Res) && Tearing;
    }
}