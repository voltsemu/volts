#include "DX12Support.h"

#include <comdef.h>

namespace Volts::PS3::RSX::DX12
{
    DX12Device::DX12Device(IDXGIAdapter1* Adapt)
        : Adapter(Adapt)
    {
        if(!!Adapt)
            Adapt->GetDesc1(&Descriptor);
    }

    DX12Device::~DX12Device()
    {
        // TODO: fix memory leaks
        // delete Adapter;
        // delete Descriptor;
    }

    std::wstring DX12Device::Name() const
    {
        return Descriptor.Description;
    }

    bool CanTear()
    {
        if(DX12::ComPtr<DX12::Factory> Factory; SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&Factory))))
            if(DX12::ComPtr<DX12::Factory5> Factory5; SUCCEEDED(Factory.As(&Factory5)))
                if(bool Tearing; FAILED(Factory5->CheckFeatureSupport(
                    DXGI_FEATURE_PRESENT_ALLOW_TEARING,
                    &Tearing,
                    sizeof(Tearing)
                )))
                    return Tearing;

        return false;
    }
}