#include <svl/platform.h>

#if SYS_WINDOWS
#   define VK_USE_PLATFORM_WIN32_KHR
#   define GLFW_EXPOSE_NATIVE_WIN32
#elif SYS_OSX
#   define GLFW_EXPOSE_NATIVE_COCOA
#   define VK_USE_PLATFORM_MACOS_MVK
#endif

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "volts.h"

#include <vector>
#include <string>
#include <tuple>

#include <glslang/Public/ShaderLang.h>
#include <SPIRV/GlslangToSpv.h>

#define VK_SALLOC(type, num) (type*)salloc(sizeof(type) * num)

#define VK_ENSURE(...) { if(VkResult res = (__VA_ARGS__); res) { spdlog::error("[{}:{}] = {}", __FILE__, __LINE__, volts::rsx::vk::to_string(res)); } }

namespace volts::rsx::vk
{
    
#define STR_CASE(val) case val: return #val;
    
    const char* to_string(VkResult res)
    {
        switch(res)
        {
        STR_CASE(VK_SUCCESS);
        STR_CASE(VK_NOT_READY);
        STR_CASE(VK_TIMEOUT);
        STR_CASE(VK_EVENT_SET);
        STR_CASE(VK_EVENT_RESET);
        STR_CASE(VK_INCOMPLETE);
        STR_CASE(VK_ERROR_OUT_OF_HOST_MEMORY);
        STR_CASE(VK_ERROR_OUT_OF_DEVICE_MEMORY);
        STR_CASE(VK_ERROR_INITIALIZATION_FAILED);
        STR_CASE(VK_ERROR_DEVICE_LOST);
        STR_CASE(VK_ERROR_MEMORY_MAP_FAILED);
        STR_CASE(VK_ERROR_LAYER_NOT_PRESENT);
        STR_CASE(VK_ERROR_EXTENSION_NOT_PRESENT);
        STR_CASE(VK_ERROR_FEATURE_NOT_PRESENT);
        STR_CASE(VK_ERROR_INCOMPATIBLE_DRIVER);
        STR_CASE(VK_ERROR_TOO_MANY_OBJECTS);
        STR_CASE(VK_ERROR_FORMAT_NOT_SUPPORTED);
        STR_CASE(VK_ERROR_FRAGMENTED_POOL);
        STR_CASE(VK_ERROR_OUT_OF_POOL_MEMORY);
        STR_CASE(VK_ERROR_INVALID_EXTERNAL_HANDLE);
        STR_CASE(VK_ERROR_SURFACE_LOST_KHR);
        STR_CASE(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
        STR_CASE(VK_SUBOPTIMAL_KHR);
        STR_CASE(VK_ERROR_OUT_OF_DATE_KHR);
        STR_CASE(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
        STR_CASE(VK_ERROR_VALIDATION_FAILED_EXT);
        STR_CASE(VK_ERROR_INVALID_SHADER_NV);
        STR_CASE(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
        STR_CASE(VK_ERROR_NOT_PERMITTED_EXT);
        STR_CASE(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
        STR_CASE(VK_ERROR_FRAGMENTATION_EXT);
        STR_CASE(VK_ERROR_INVALID_DEVICE_ADDRESS_EXT);
        STR_CASE(VK_RESULT_MAX_ENUM);
        default:
            return "VK_ERROR_UNKNOWN";
        }
    }
}
