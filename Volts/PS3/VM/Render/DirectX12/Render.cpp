#include "Render.h"

namespace Volts::PS3::RSX
{
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
        Count = 0;
        return nullptr;
    }
}
