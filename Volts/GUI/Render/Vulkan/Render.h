#pragma once

#include "Render/Render.h"

#include "imgui/imgui.h"
#include "imgui/examples/imgui_impl_vulkan.h"

#include "Support.h"

namespace Volts::RSX
{
    struct Vulkan : Render
    {
        Vulkan();
        virtual ~Vulkan();
        virtual void Attach(GUI::Frame* Handle) override;
        virtual void Detach() override;
        virtual const String Name() const override { return "Vulkan"; }
        virtual const String Description() const override { return "Vulkan3D"; }
        virtual Device* Devices(U32* Count) override;
        virtual void UpdateVSync(bool NewMode) override;

        virtual void BeginRender() override;
        virtual void PresentRender() override;

        virtual void Resize(GUI::Size NewSize) override;

        virtual void Windowed() override;
        virtual void Borderless() override;
        virtual void Fullscreen() override;
    private:
        GUI::Frame* Frame;

        void CreateInstance();
        VkInstance Instance;

        void QueryDevices();
        Array<VulkanSupport::VulkanDevice> Devices;

        // Queue family indecies
        U32 GraphicsIndex = 0;
        U32 ComputeIndex = 0;
        U32 TransferIndex = 0;

        void SetDevice(VulkanSupport::VulkanDevice NewDevice);
        VulkanSupport::VulkanDevice CurrentDevice;
        VkDevice LogicalDevice;

        VkQueue Queue = VK_NULL_HANDLE;
    };
}