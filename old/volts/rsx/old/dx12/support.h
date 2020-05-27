#pragma once

#include <comdef.h>
#include <spdlog/spdlog.h>

#include <glm/glm.hpp>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>

#include "d3dx12.h"

#include <wrl.h>

#define DX_ENSURE(...) { if(HRESULT hr = (__VA_ARGS__); FAILED(hr)) { spdlog::error("[{}:{}] = {}", __FILE__, __LINE__, _com_error(hr).ErrorMessage()); std::abort(); } }

namespace volts::rsx
{
    using namespace Microsoft::WRL;

    bool can_tear()
    {
        ComPtr<IDXGIFactory4> factory4;
        HRESULT res = CreateDXGIFactory1(IID_PPV_ARGS(&factory4));
        bool tear = false;

        if(SUCCEEDED(res))
        {
            ComPtr<IDXGIFactory5> factory5;
            res = factory4.As(&factory5);

            if(SUCCEEDED(res))
            {
                res = factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &tear, sizeof(tear));
            }   
        }

        return SUCCEEDED(res) && tear;
    }
}