#include "Support.h"

#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d12.h>

namespace Volts::DX12Support
{
    using Microsoft::WRL::ComPtr;
    bool CanTear()
    {
        ComPtr<IDXGIFactory6> Factory;
        VALIDATE(CreateDXGIFactory1(IID_PPV_ARGS(&Factory)));
        bool Tearing = false;
        VALIDATE(Factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &Tearing, sizeof(Tearing)));

        return Tearing;
    }
}