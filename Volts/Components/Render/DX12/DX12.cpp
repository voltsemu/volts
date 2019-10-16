#include "DX12.h"

#include "imgui/imgui.h"

#include "imgui/examples/imgui_impl_dx12.h"

namespace Volts::Render
{
    static DX12* Singleton = new DX12();
    DX12::DX12()
    {
        Emulator::Get()->Render.Register(this);

        UINT FactoryFlags = 0;

        {
            Ptr<ID3D12Debug> Debugger;
            VALIDATE(D3D12GetDebugInterface(IID_PPV_ARGS(&Debugger)));
            Debugger->EnableDebugLayer();
            FactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }

        VALIDATE(CreateDXGIFactory2(FactoryFlags, IID_PPV_ARGS(&Factory)));

        IDXGIAdapter1* Adapter;

        for(U32 I = 0; Factory->EnumAdapters1(I, &Adapter) != DXGI_ERROR_NOT_FOUND; I++)
            DeviceList.push_back(DX12Device(Adapter));
    }

    void DX12::SetDevice(U32 Index)
    {

    }

    void DX12::Attach()
    {
        I32 Width, Height;
        glfwGetWindowSize(Emulator::Get()->Frame, &Width, &Height);

        Viewport = {
            0.f, 0.f,
            (F32)Width, (F32)Height
        };

        Scissor = {
            0, 0,
            Width, Height
        };

        LoadPipeline()
        LoadData();

        ImGui_ImplDX12_Init(
            Device.Get(),
            FrameCount,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            SRVHeap->GetCPUDescriptorHandleForHeapStart(),
            SRVHeap->GetGPUDescriptorHandleForHeapStart()
        );
    }

    void DX12::Detach()
    {

    }

    void DX12::Begin()
    {

    }

    void DX12::End()
    {

    }


}