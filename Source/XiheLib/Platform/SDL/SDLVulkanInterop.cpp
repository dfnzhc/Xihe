/**
 * @File SDLVulkanInterop.cpp
 * @Author dfnzhc (https://github.com/dfnzhc)
 * @Date 2025/8/12
 * @Brief This file is part of Xihe.
 */

#include <vector>

#include "Core/Base/Defines.hpp"
#include "Platform/Window.hpp"
#include "Platform/VulkanInterop.hpp"

XIHE_PUSH_WARNING
XIHE_CLANG_DISABLE_WARNING("-Wreserved-macro-identifier")
XIHE_CLANG_DISABLE_WARNING("-Wdocumentation-unknown-command")
XIHE_CLANG_DISABLE_WARNING("-Wunsafe-buffer-usage")
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

namespace xihe {
bool GetVulkanRequiredInstanceExtensions(IWindow* /*window*/, std::vector<const char*>& outExtensions)
{
    outExtensions.clear();
    Uint32 count                          = 0;
    const char* const* instanceExtensions = SDL_Vulkan_GetInstanceExtensions(&count);
    if (instanceExtensions == nullptr)
    {
        return false;
    }
    outExtensions.reserve(count);
    for (Uint32 i = 0; i < count; ++i)
    {
        outExtensions.push_back(instanceExtensions[i]);
    }
    return true;
}

bool CreateVulkanSurfaceFromWindow(IWindow* window, void* vkInstance, void** outSurface)
{
    if (!window || !vkInstance || !outSurface)
    {
        return false;
    }
    auto* sdlWindow      = reinterpret_cast<SDL_Window*>(window->nativeHandle());
    VkInstance instance  = reinterpret_cast<VkInstance>(vkInstance);
    VkSurfaceKHR surface = nullptr;
    if (!SDL_Vulkan_CreateSurface(sdlWindow, instance, nullptr, &surface))
    {
        return false;
    }
    *outSurface = reinterpret_cast<void*>(surface);
    return true;
}
} // namespace xihe

XIHE_POP_WARNING
