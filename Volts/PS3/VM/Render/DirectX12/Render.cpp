#include "Render.h"

namespace Volts::PS3::RSX
{
    using namespace Cthulhu;

    DX12Device::DX12Device(IDXGIAdapter1* Adapt)
        : Adapter(Adapt)
    {
        Adapt->GetDesc1(Descriptor);
    }

    DX12Device::~DX12Device()
    {
        delete Adapter;
        delete Descriptor;
    }

    const char* DX12Device::Name() const
    {
        return (const char*)Descriptor->Description;
    }

    DirectX12::DirectX12()
    {
        MessageBox(nullptr, "DirectX12 Ctor", "Volts", 0);
    }

    InitError DirectX12::Init()
    {
        // get all the things we can use as a render device
        DX12::Factory* Factory = nullptr;
        CreateDXGIFactory1(__uuidof(DX12::Factory), (void**)&Factory);
        DX12::Adapter* Adapt = nullptr;
        for(U32 I = 0; Factory->EnumAdapters1(I, &Adapt) != DXGI_ERROR_NOT_FOUND; I++)
            RenderDevices.Append(Adapt);

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
