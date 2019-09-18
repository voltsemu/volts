#include "Render.h"
#include <vector>

#define VALIDATE(...) if(FAILED(__VA_ARGS__)) return false;

namespace Volts::RSX
{
    void DX12::Start(GUI::Frame* Handle)
    {

    }

    void DX12::Detach()
    {

    }

    bool DX12::LoadPipeline()
    {
        UINT FactoryFlags = 0;

        Ptr<IDXGIFactory4> Factory;
        VALIDATE(CreateDXGIFactory2(FactoryFlags, IID_PPV_ARGS(&Factory)));

        Ptr<IDXGIAdapter1> Adapter;
        std::vector<Ptr<IDXGIAdapter1>> Adapters;

        for(U32 I = 0; Factory->EnumAdapters1(I, &Adapter) != DXGI_ERROR_NOT_FOUND; I++)
        {
            Adapters.push_back(Adapter);
        }

        return true;
    }

    bool DX12::Setup()
    {
        return true;
    }
}