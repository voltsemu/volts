#include "DX12Support.h"

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

    ComPtr<Device> CreateDevice(ComPtr<Adapter> Adapt)
    {
        ComPtr<Device> Device;
        D3D12CreateDevice(Adapt.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&Device));
        return Device;
    }
}