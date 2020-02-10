#pragma once

#include <d3d12.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <tchar.h>

#include <system_error>

#include <spdlog/spdlog.h>

#include <wrl.h>
using namespace Microsoft::WRL;

#define VALIDATE(...) { auto hr = (__VA_ARGS__); if(FAILED(hr)) { spdlog::error("d3d12 error {} at {}:{}", std::system_category().message(hr), __FILE__, __LINE__); /* TODO exit */ } }

namespace volts::rsx::dxutil
{
    struct dx12device : device
    {
        dx12device(IDXGIAdapter1* h)
            : handle(h)
        {
            handle->GetDesc1(&desc);
        }

        virtual ~dx12device() override {}

        virtual const std::wstring& name() const override
        {
            return std::wstring(desc.Description);
        }

        IDXGIAdapter1* handle;
        DXGI_ADAPTER_DESC1 desc;
    };

    bool can_tear(IDXGIFactory5* factory)
    {
        bool tear = false;
        return factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &tear, sizeof(tear)) && tear;
    }

}