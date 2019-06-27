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

    ComPtr<Queue> CreateCommandQueue(ComPtr<Device> Dev)
    {
        ComPtr<Queue> CommandQueue;

        QueueDescriptor Desc = {};
        Desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        Desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

        DX_CHECK(Dev->CreateCommandQueue(&Desc, IID_PPV_ARGS(&CommandQueue)));

        return CommandQueue;
    }

    bool TearingSupport()
    {
        bool CanTear = false;

        if(ComPtr<Factory> Factory; SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&Factory))))
            if(ComPtr<Factory5> Factory5; SUCCEEDED(Factory.As(&Factory5)))
                if(FAILED(Factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &CanTear, sizeof(CanTear))))
                    CanTear = false;

        return CanTear;
    }
}