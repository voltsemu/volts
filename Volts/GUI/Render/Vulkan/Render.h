#pragma once

#include "Render/Render.h"

#include "Support.h"

namespace Volts::RSX
{
    struct Vulkan : Render
    {
        Vulkan();
        virtual ~Vulkan() override;
        virtual void Attach(GUI::Frame* Handle) override;
        virtual void Detach() override;

        virtual const char* Name() const override
        {
#if OS_APPLE
            return "MoltenVK";
#else
            return "Vulkan";
#endif
        }
        virtual const char* Description() const override { return "Vulkan3D"; }

        virtual Device* Devices(U32* Count) override;
        virtual void SetDevice(RSX::Device* Device) override;

        virtual void UpdateVSync(bool NewMode) override;

        virtual void BeginRender() override;
        virtual void PresentRender() override;

        virtual void Resize(GUI::Size NewSize) override;

        virtual void Windowed() override;
        virtual void Borderless() override;
        virtual void Fullscreen() override;
    private:
        // global data
        GUI::Frame* Frame;

        void CreateInstance();
        void DeleteInstance();
        VkInstance Instance;
        
#if VVKDEBUG && !OS_APPLE
        VkDebugUtilsMessengerEXT DebugMessenger;
#endif

        void CreateDevices();
        void DeleteDevices();
        VulkanSupport::VulkanDevice* DeviceList;
        U32 DeviceCount;
        U32 DeviceIndex = 0;

        void CreateSurface();
        void DeleteSurface();
        VkSurfaceKHR Surface;

        // device specific data
        void CreateQueue();
        void DeleteQueue();
        VkQueue Queue;

        void CreateSwapchain();
        void DeleteSwapchain();
        VkSwapchainKHR Swap;

        void SetupVulkan();
        void CreatePipeline();
    };
}