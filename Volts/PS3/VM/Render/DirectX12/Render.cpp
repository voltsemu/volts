#include "Render.h"

namespace Volts::PS3::RSX
{
    using namespace Cthulhu;

    DirectX12::DirectX12()
    {
        Render::Register(this);
        // get all the things we can use as a render device
        DX12::Factory* Factory = nullptr;
        CreateDXGIFactory1(__uuidof(DX12::Factory), (void**)&Factory);
        DX12::Adapter* Adapt = nullptr;
        for(U32 I = 0; Factory->EnumAdapters1(I, &Adapt) != DXGI_ERROR_NOT_FOUND; I++)
            RenderDevices.Append(DX12::DX12Device(Adapt));

#if VDEBUG
        DX12::ComPtr<DX12::Debug> DebugInterface;
        D3D12GetDebugInterface(IID_PPV_ARGS(&DebugInterface));
        DebugInterface->EnableDebugLayer();
#endif
    }

    InitError DirectX12::Init()
    {
        return InitError::Ok;
    }

    void DirectX12::DeInit()
    {

    }

    bool DirectX12::Supported() const
    {
        return false;
    }

    RenderDevice* DirectX12::Devices(unsigned& Count) const
    {
        Count = RenderDevices.Len();
        return RenderDevices.Data();
    }

    static DirectX12* DX12Singleton = new DirectX12();
}
